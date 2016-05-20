/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <gio/gio.h>

#include "balde.h"
#include "app.h"
#include "cgi.h"
#include "exceptions.h"
#include "requests.h"
#include "utils.h"
#include "scgi.h"


static gboolean
balde_incoming_callback(GThreadedSocketService *service,
    GSocketConnection *connection, GObject *source_object, gpointer user_data)
{
    GError *error = NULL;

    GInputStream *istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    balde_app_t *app = user_data;

    balde_request_env_t *env = balde_scgi_parse_request(app, istream);
    if (env == NULL)
        balde_abort_set_error(app, 400);

    GString *response = balde_app_main_loop(app, env, balde_response_render,
        NULL);

    GOutputStream *ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));

    g_output_stream_write_all(ostream, response->str, response->len, NULL, NULL, &error);
    g_string_free(response, TRUE);
    if (error != NULL) {
        g_printerr("Failed to send: %s\n", error->message);
        g_error_free(error);
        error = NULL;
    }

    g_io_stream_close(G_IO_STREAM(connection), NULL, &error);
    if (error != NULL) {
        g_printerr("Failed to close connection: %s\n", error->message);
        g_error_free(error);
    }

    return TRUE;
}


static void
balde_filter_headers(gchar* key, gchar* value, GHashTable *headers)
{
    gchar *key2 = balde_parse_header_name_from_envvar(key);
    if (key2 != NULL)
        g_hash_table_insert(headers, key2, g_strdup(value));
}


typedef enum {
    NETSTRING_LEN = 1,
    NETSTRING_HEADER_KEY,
    NETSTRING_HEADER_VALUE,
} balde_scgi_netstring_state_t;


balde_request_env_t*
balde_scgi_parse_request(balde_app_t *app, GInputStream *istream)
{
    GDataInputStream *data = g_data_input_stream_new(istream);

    gchar *key = NULL;
    gchar *value = NULL;
    GString *buf = g_string_new("");
    GHashTable *env = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
        g_free);

    balde_scgi_netstring_state_t state = NETSTRING_LEN;

    guchar c = 0;
    guint64 netstring_len = 0;
    guint64 start = 0;
    guint64 current = 0;

    while (TRUE) {
        c = g_data_input_stream_read_byte(data, NULL, NULL);
        g_string_append_c(buf, c);

        switch(state) {

            case NETSTRING_LEN:
                if (c >= '0' && c <= '9')
                    break;
                if (c != ':') {
                    g_free(key);
                    g_free(value);
                    g_string_free(buf, TRUE);
                    return NULL;
                }
                key = g_strndup(buf->str + start, buf->len - start);
                netstring_len = g_ascii_strtoull(key, NULL, 10);
                g_free(key);
                key = NULL;
                state = NETSTRING_HEADER_KEY;
                start = buf->len;
                break;

            case NETSTRING_HEADER_KEY:
                if (c != '\0')
                    break;
                key = g_strndup(buf->str + start, buf->len - start);
                state = NETSTRING_HEADER_VALUE;
                start = buf->len;
                break;

            case NETSTRING_HEADER_VALUE:
                if (c != '\0' && current != netstring_len)
                    break;
                value = g_strndup(buf->str + start, buf->len - start);
                g_hash_table_replace(env, key, value);
                key = NULL;
                value = NULL;
                state = NETSTRING_HEADER_KEY;
                start = buf->len;
                break;

        }

        if (state != NETSTRING_LEN) {
            if (current == netstring_len)
                break;
            current++;
        }
    }

    g_free(key);
    g_free(value);
    g_string_free(buf, TRUE);

    if (',' != g_data_input_stream_read_byte(data, NULL, NULL)) {
        return NULL;
    }

    GString *body = NULL;
    GHashTable *headers = g_hash_table_new_full(g_str_hash, g_str_equal,
        g_free, g_free);

    g_hash_table_foreach(env, (GHFunc) balde_filter_headers, headers);

    guint64 content_length = 0;
    gchar *clen_str = g_hash_table_lookup(env, "CONTENT_LENGTH");

    if (clen_str != NULL) {
        g_hash_table_replace(headers, g_strdup("content-length"),
            g_strdup(clen_str));
        content_length = balde_cgi_parse_content_length(clen_str);
    }

    gchar *ctype_str = g_hash_table_lookup(env, "CONTENT_TYPE");
    if (ctype_str != NULL)
        g_hash_table_replace(headers, g_strdup("content-type"),
            g_strdup(ctype_str));

    if (content_length > 0) {
        gchar body_[1024];
        gssize real_content_length = 0;
        gssize size;
        gssize to_read = sizeof(body_);
        body = g_string_new(NULL);
        while (real_content_length < content_length) {
            if (content_length - real_content_length < to_read)
                to_read = content_length - real_content_length;
            size = g_input_stream_read(G_INPUT_STREAM(data), body_, to_read,
                NULL, NULL);
            g_string_append_len(body, body_, size);
            real_content_length += size;
        }
    }

    g_object_unref(data);

    balde_request_env_t *req_env = g_new(balde_request_env_t, 1);
    req_env->server_name = g_strdup(g_hash_table_lookup(env, "SERVER_NAME"));
    req_env->script_name = g_strdup(g_hash_table_lookup(env, "SCRIPT_NAME"));
    gchar *path_info = g_strdup(g_hash_table_lookup(env, "PATH_INFO"));
    req_env->query_string = NULL;
    if (path_info == NULL) {  // this is dumb, but its how nginx works
        path_info = g_strdup(g_hash_table_lookup(env, "REQUEST_URI"));

        gchar **pieces = g_strsplit(path_info, "?", 2);
        req_env->path_info = g_strdup(pieces[0]);
        if (pieces[1] != NULL)
            req_env->query_string = g_strdup(pieces[1]);
        // FIXME: we could just g_free(pieces) here, and avoid the g_strdup
        // calls above, but I'm afraid of memory leak due to weird memory
        // optimizations from glib.
        g_strfreev(pieces);
    }
    else {
        req_env->path_info = path_info;
        char *qs = g_hash_table_lookup(env, "QUERY_STRING");
        if (qs != NULL)
            req_env->query_string = g_strdup(qs);
    }

    req_env->request_method = g_strdup(g_hash_table_lookup(env, "REQUEST_METHOD"));
    req_env->headers = headers;
    req_env->body = body;
    req_env->https = g_hash_table_lookup(env, "HTTPS") != NULL;

    g_hash_table_destroy(env);

    return req_env;
}


void
balde_scgi_run(balde_app_t *app, const gchar *host, gint16 port,
    gint max_threads)
{
    // TODO: add unix socket support
    GError *error = NULL;
    const gchar *final_host = host != NULL ? host : "127.0.0.1";
    g_printerr(" * Running SCGI on %s:%d (threads: %d)\n", final_host, port,
        max_threads);
    GSocketService *service = g_threaded_socket_service_new(max_threads);
    GInetAddress* addr_host = g_inet_address_new_from_string(final_host);
    GSocketAddress *address = g_inet_socket_address_new(addr_host, port);
    g_socket_listener_add_address(G_SOCKET_LISTENER(service), address,
        G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, NULL, NULL, &error);
    if (error != NULL) {
        g_printerr("Failed to listen: %s\n", error->message);
        g_error_free(error);
        g_object_unref(service);
        g_object_unref(addr_host);
        g_object_unref(address);
        return;
    }
    g_signal_connect(service, "run", G_CALLBACK(balde_incoming_callback), app);
    g_socket_service_start(service);
    g_object_unref(service);
    g_object_unref(addr_host);
    g_object_unref(address);
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
}
