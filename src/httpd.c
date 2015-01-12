/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
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
#include "datetime.h"
#include "exceptions.h"
#include "httpd.h"
#include "wrappers.h"


balde_httpd_parser_data_t*
balde_httpd_parse_request(balde_app_t *app, GInputStream *istream)
{
    GDataInputStream *data = g_data_input_stream_new(istream);
    g_data_input_stream_set_newline_type(data, G_DATA_STREAM_NEWLINE_TYPE_ANY);
    gchar *line = g_data_input_stream_read_line(data, NULL, NULL, NULL);

    if (line == NULL) {
        g_object_unref(data);
        return NULL;
    }

    gchar *request_line = g_strdup(line);
    gchar *request_method = NULL;
    gchar *path_info = NULL;
    gchar *query_string = NULL;
    gchar *key = NULL;
    gchar *value = NULL;
    gchar *body = NULL;
    GHashTable *headers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    gchar **pieces = g_strsplit(line, " ", 3);
    g_free(line);

    if (g_strv_length(pieces) == 3 && g_str_has_prefix(pieces[2], "HTTP/1.")) {
        request_method = g_strdup(pieces[0]);
        gchar **pieces2 = g_strsplit(pieces[1], "?", 2);
        path_info = g_strdup(pieces2[0]);
        query_string = g_strdup(pieces2[1] == NULL ? "" : pieces2[1]);
        g_strfreev(pieces2);
    }
    g_strfreev(pieces);

    line = g_data_input_stream_read_line(data, NULL, NULL, NULL);
    while (line[0] != '\0') {
        pieces = g_strsplit(line, ":", 2);
        g_free(line);
        if (g_strv_length(pieces) != 2)
            continue;  // just ignore wrong headers :/
        key = g_ascii_strdown(g_strstrip(pieces[0]), -1);
        value = g_strdup(g_strstrip(pieces[1]));
        g_hash_table_insert(headers, key, value);
        g_strfreev(pieces);
        line = g_data_input_stream_read_line(data, NULL, NULL, NULL);
    }
    g_free(line);

    guint64 content_length = 0;
    const gchar* clen_str = g_hash_table_lookup(headers, "content-length");
    if (clen_str != NULL)
        content_length = g_ascii_strtoull(clen_str, NULL, 10);

    if (content_length > 0) {
        gchar body_[content_length];
        g_input_stream_read(G_INPUT_STREAM(data), body_, sizeof(body_), NULL, NULL);
        body = g_strndup(body_, content_length);
    }

    g_object_unref(data);

    balde_request_env_t *env = g_new(balde_request_env_t, 1);
    env->script_name = NULL;
    env->path_info = path_info;
    env->request_method = request_method;
    env->query_string = query_string;
    env->headers = headers;
    env->content_length = content_length;
    env->body = body;

    balde_httpd_parser_data_t *parser_data = g_new(balde_httpd_parser_data_t, 1);
    parser_data->env = env;
    parser_data->request_line = request_line;

    return parser_data;
}


GString*
balde_httpd_response_render(balde_response_t *response, const gboolean with_body)
{
    if (response == NULL)
        return NULL;
    GString *str = g_string_new("");
    gchar *n = g_ascii_strup(
        balde_exception_get_name_from_code(response->status_code), -1);
    g_string_append_printf(str, "HTTP/1.0 %d %s\r\n", response->status_code, n);
    g_free(n);
    gchar *len = g_strdup_printf("%zu", response->priv->body->len);
    GDateTime *dt = g_date_time_new_now_utc();
    gchar *date = balde_datetime_rfc5322(dt);
    g_date_time_unref(dt);
    balde_response_set_header(response, "Date", date);
    balde_response_set_header(response, "Connection", "close");
    balde_response_set_header(response, "Content-Length", len);
    g_free(date);
    g_free(len);
    if (g_hash_table_lookup(response->priv->headers, "content-type") == NULL)
        balde_response_set_header(response, "Content-Type", "text/html; charset=utf-8");
    g_hash_table_foreach(response->priv->headers, (GHFunc) balde_header_render, str);
    g_string_append(str, "\r\n");
    if (with_body)
        g_string_append_len(str, response->priv->body->str,
            response->priv->body->len);
    return str;
}


static gboolean
balde_incoming_callback(GThreadedSocketService *service,
    GSocketConnection *connection, GObject *source_object, gpointer user_data)
{
    GError *error = NULL;
    gchar *remote_ip = NULL;
    GSocketAddress *remote_socket = g_socket_connection_get_remote_address(
        connection, NULL);
    GInetAddress *remote_addr;
    switch (g_socket_address_get_family(remote_socket)) {
        case G_SOCKET_FAMILY_IPV4:
        case G_SOCKET_FAMILY_IPV6:
            remote_addr = g_inet_socket_address_get_address(
                (GInetSocketAddress*) remote_socket);
            remote_ip = g_inet_address_to_string(remote_addr);
            break;
        case G_SOCKET_FAMILY_UNIX:
        case G_SOCKET_FAMILY_INVALID:
            // -EDONTCARE
            break;
    }
    g_object_unref(remote_socket);
    balde_app_t *app = user_data;
    GInputStream *istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    balde_httpd_parser_data_t *parser_data = balde_httpd_parse_request(app, istream);
    if (parser_data == NULL)
        goto point2;
    balde_http_exception_code_t status_code = BALDE_HTTP_INTERNAL_SERVER_ERROR;
    GString *response = balde_app_main_loop(app, parser_data->env,
        balde_httpd_response_render, &status_code);
    GOutputStream *ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));
    g_output_stream_write(ostream, response->str, response->len, NULL, &error);
    g_string_free(response, TRUE);
    if (error != NULL) {
        g_printerr("Failed to send: %s\n", error->message);
        g_error_free(error);
        goto point1;
    }
    GDateTime *dt = g_date_time_new_now_local();
    gchar *dt_format = balde_datetime_logging(dt);
    g_date_time_unref(dt);
    g_printerr("%s - - [%s] \"%s\" %d\n", remote_ip, dt_format,
        parser_data->request_line, status_code);
    g_free(dt_format);
    g_io_stream_close(G_IO_STREAM(connection), NULL, &error);
    if (error != NULL) {
        g_printerr("Failed to close connection: %s\n", error->message);
        g_error_free(error);
    }
point1:
    g_free(parser_data->request_line);
    g_free(parser_data);
    g_free(remote_ip);
point2:
    return TRUE;
}


void
balde_httpd_run(balde_app_t *app, const gchar *host, gint16 port,
    gint max_threads)
{
    GError *error = NULL;
    const gchar *final_host = host != NULL ? host : "127.0.0.1";
    g_printerr(" * Running on http://%s:%d/ (threads: %d)\n", final_host, port,
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
