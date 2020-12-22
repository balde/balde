/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <gio/gio.h>

#include "../balde.h"
#include "../app.h"
#include "../exceptions.h"
#include "../requests.h"
#include "../sapi.h"
#include "cgi.h"
#include "fcgi.h"

typedef enum {
    FCGI_BEGIN_REQUEST = 1,
    FCGI_ABORT_REQUEST,
    FCGI_END_REQUEST,
    FCGI_PARAMS,
    FCGI_STDIN,
    FCGI_STDOUT,
    FCGI_STDERR,
    FCGI_DATA,
    FCGI_GET_VALUES,
    FCGI_GET_VALUES_RESULT,
    FCGI_UNKNOWN_TYPE,
} balde_sapi_fcgi_record_type_t;

typedef enum {
    FCGI_RESPONDER = 1,
    FCGI_AUTHORIZER,
    FCGI_FILTER,
} balde_sapi_fcgi_role_t;

typedef enum {
    FCGI_REQUEST_COMPLETE,
    FCGI_CANT_MPX_CONN,
    FCGI_OVERLOADED,
    FCGI_UNKNOWN_ROLE,
} balde_sapi_fcgi_protocol_status_t;

typedef struct {
    guint8 version;
    guint8 type;
    guint16 request_id;
    guint16 content_length;
    guint8 padding_length;
    guint8 reserved;
} balde_sapi_fcgi_header_t;

typedef struct {
    balde_sapi_fcgi_header_t *header;
    guint8 buffer[0xffff + 0xff];
} balde_sapi_fcgi_record_t;

typedef struct {
    guint16 role;
    guint8 flags;
    guint8 reserved[5];
} balde_sapi_fcgi_begin_request_body_t;

typedef struct {
    GOutputStream *ostream;
    GHashTable *requests;
    GMutex mutex;
    GMutex mutex_cond;
    GCond cond;
} balde_sapi_fcgi_connection_t;

typedef struct {
    guint16 id;
    guint8 flags;
    GByteArray *params;
    GByteArray *body;
    gboolean processed;
} balde_sapi_fcgi_request_t;

typedef struct {
    balde_app_t *app;
    GThreadPool *pool;
} balde_sapi_fcgi_user_data_t;

typedef struct {
    balde_sapi_fcgi_connection_t *connection;
    balde_sapi_fcgi_request_t *request;
} balde_sapi_fcgi_thread_user_data_t;

static const guint8 padding[8] = {0, 0, 0, 0, 0, 0, 0, 0};


static balde_sapi_fcgi_header_t*
balde_sapi_fcgi_read_header(GInputStream *istream)
{
    GError *err = NULL;

    balde_sapi_fcgi_header_t *rv = g_new(balde_sapi_fcgi_header_t, 1);
    gsize r;
    if (!g_input_stream_read_all(istream, rv, sizeof(balde_sapi_fcgi_header_t),
        &r, NULL, &err))
    {
        g_printerr("fcgi: error: received invalid FastCGI header, dropping "
            "connection.\n");
        g_free(rv);
        return NULL;
    }

    // as this is the first read we do to get any record, this usually means
    // that we got EOF because the server sent keep_conn, but closed connection.
    // no need to make noise about this :)
    if (r == 0) {
        g_free(rv);
        return NULL;
    }

    // fix byte order
    rv->request_id = g_ntohs(rv->request_id);
    rv->content_length = g_ntohs(rv->content_length);

    return rv;
}


static void
balde_sapi_fcgi_request_free(balde_sapi_fcgi_request_t *r)
{
    if (r == NULL)
        return;
    if (r->params != NULL)
        g_byte_array_free(r->params, TRUE);
    if (r->body != NULL)
        g_byte_array_free(r->body, TRUE);
    g_free(r);
}


static void
balde_sapi_fcgi_record_free(balde_sapi_fcgi_record_t *r)
{
    if (r == NULL)
        return;
    g_free(r->header);
    g_free(r);
}


balde_request_env_t*
balde_sapi_fcgi_parse_request(GByteArray *params, GByteArray *body)
{
    if (params == NULL || body == NULL)
        return NULL;

    balde_request_env_t *rv = g_new(balde_request_env_t, 1);
    rv->server_name = NULL;
    rv->script_name = NULL;
    rv->path_info = NULL;
    rv->request_method = NULL;
    rv->query_string = NULL;
    rv->https = FALSE;
    rv->headers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    rv->body = NULL;

    gint i = 0;
    gint key_len;
    gint value_len;

    while (i < params->len) {

        // key length
        if (params->data[i] >> 7 == 0) {
            key_len = params->data[i];
            i++;
        }
        else {
            if ((params->len - i) < 4) {
                balde_request_env_free(rv);
                return NULL;
            }
            key_len =
                ((params->data[i] & 0x7f) << 24) +
                 (params->data[i + 1] << 16) +
                 (params->data[i + 2] << 8) +
                  params->data[i + 3];
            i += 4;
        }

        if (i >= params->len) {
            balde_request_env_free(rv);
            return NULL;
        }

        // value length
        if (params->data[i] >> 7 == 0) {
            value_len = params->data[i];
            i++;
        }
        else {
            if ((params->len - i) < 4) {
                balde_request_env_free(rv);
                return NULL;
            }
            value_len =
                ((params->data[i] & 0x7f) << 24) +
                 (params->data[i + 1] << 16) +
                 (params->data[i + 2] << 8) +
                  params->data[i + 3];
            i += 4;
        }

        char *key = g_strdup_printf("%.*s", key_len, params->data + i);
        i += key_len;
        char *value = g_strdup_printf("%.*s", value_len, params->data + i);
        i += value_len;

        if (0 == g_strcmp0(key, "SERVER_NAME")) {
            rv->server_name = value;
            g_free(key);
        }
        else if (0 == g_strcmp0(key, "SCRIPT_NAME")) {
            rv->script_name = value;
            g_free(key);
        }
        else if (0 == g_strcmp0(key, "PATH_INFO")) {
            rv->path_info = value;
            g_free(key);
        }
        else if (0 == g_strcmp0(key, "REQUEST_METHOD")) {
            rv->request_method = value;
            g_free(key);
        }
        else if (0 == g_strcmp0(key, "QUERY_STRING")) {
            rv->query_string = value;
            g_free(key);
        }
        else if (0 == g_strcmp0(key, "HTTPS")) {
            rv->https = TRUE;
            g_free(key);
            g_free(value);
        }
        else {
            char *header_key = balde_parse_header_name_from_envvar(key);
            if (header_key != NULL) {
                g_hash_table_replace(rv->headers, header_key, value);
            }
            else {
                g_free(value);
            }
            g_free(key);
        }
    }

    const char *clen_str = g_hash_table_lookup(rv->headers, "content-length");
    guint64 clen = balde_sapi_cgi_parse_content_length(clen_str);
    if (clen > 0 && clen <= body->len) {
        rv->body = g_string_new_len((char*) body->data, clen);
    }

    return rv;
}


void
balde_sapi_fcgi_add_record(GByteArray *ba, guint16 request_id, guint8 type,
    guint8 *data, guint16 data_len)
{
    data_len = data == NULL ? 0 : data_len;
    guint8 padding_len = 8 - data_len % 8;
    padding_len = (padding_len == 8) ? 0 : padding_len;

    guint8 header[] = {
        1,                                  // version
        type,                               // type
        (guint8) (request_id >> 8) & 0xff,  // request_id high byte
        (guint8)  request_id       & 0xff,  // request_id low byte
        (guint8) (data_len   >> 8) & 0xff,  // data length high byte
        (guint8)  data_len         & 0xff,  // data length low byte
        padding_len,                        // padding
        0,                                  // reserved
    };
    g_byte_array_append(ba, header, sizeof(header));
    if (data_len > 0)
        g_byte_array_append(ba, data, data_len);
    if (padding_len > 0)
        g_byte_array_append(ba, padding, padding_len);
}


static void
balde_sapi_fcgi_send_eof(GOutputStream *stream, GByteArray *ba, guint16 request_id,
    guint8 status)
{
    if (ba == NULL)
        ba = g_byte_array_new();

    guint8 body[] = {
        0,       // app status FIXME!
        0,       // app status FIXME!
        0,       // app status FIXME!
        0,       // app status FIXME!
        status,  // protocol status
        0,       // reserved
        0,       // reserved
        0,       // reserved
    };
    balde_sapi_fcgi_add_record(ba, request_id, FCGI_END_REQUEST, body, sizeof(body));
    g_output_stream_write_all(stream, ba->data, ba->len, NULL, NULL, NULL);
    g_byte_array_free(ba, TRUE);
}


static void
balde_sapi_fcgi_send_get_values_result(GOutputStream *stream)
{
    GByteArray *ba = g_byte_array_new();

    guint8 body[] = {
        15,  // key length
        1,   // value length

        // body
        'F', 'C', 'G', 'I', '_', 'M', 'P', 'X', 'S', '_', 'C', 'O', 'N', 'N',
        'S', '1',
    };
    balde_sapi_fcgi_add_record(ba, 0, FCGI_GET_VALUES_RESULT, body, sizeof(body));
    g_output_stream_write_all(stream, ba->data, ba->len, NULL, NULL, NULL);
    g_byte_array_free(ba, TRUE);
}


static void
balde_sapi_fcgi_send_unknown_type(GOutputStream *stream, guint8 type)
{
    GByteArray *ba = g_byte_array_new();

    guint8 body[] = {
        type,  // type
        0,     // reserved
        0,     // reserved
        0,     // reserved
        0,     // reserved
        0,     // reserved
        0,     // reserved
        0,     // reserved
    };
    balde_sapi_fcgi_add_record(ba, 0, FCGI_UNKNOWN_TYPE, body, sizeof(body));
    g_output_stream_write_all(stream, ba->data, ba->len, NULL, NULL, NULL);
    g_byte_array_free(ba, TRUE);
}


static void
balde_handle_request(balde_sapi_fcgi_thread_user_data_t *data, balde_app_t *app)
{
    balde_sapi_fcgi_request_t *request = data->request;
    balde_sapi_fcgi_connection_t *connection = data->connection;

    balde_request_env_t* env = balde_sapi_fcgi_parse_request(request->params,
        request->body);
    if (env == NULL)
        balde_abort_set_error(app, 400);

    GString *response = balde_app_main_loop(app, env, balde_response_render, NULL);

    GByteArray *ba = g_byte_array_new();

    gsize current = 0;
    while (current < response->len) {
        gsize to_send = response->len - current;
        to_send = to_send > 0xffff ? 0xffff : to_send;
        balde_sapi_fcgi_add_record(ba, request->id, FCGI_STDOUT,
            (guint8*) response->str + current, to_send);
        current += to_send;
    }
    g_string_free(response, TRUE);

    balde_sapi_fcgi_add_record(ba, request->id, FCGI_STDOUT, NULL, 0);

    g_mutex_lock(&(connection->mutex));
    balde_sapi_fcgi_send_eof(connection->ostream, ba, request->id,
        FCGI_REQUEST_COMPLETE);
    g_hash_table_remove(connection->requests, GINT_TO_POINTER(request->id));
    g_mutex_unlock(&(connection->mutex));

    g_mutex_lock(&(connection->mutex_cond));
    request->processed = TRUE;
    g_cond_signal(&(connection->cond));
    g_mutex_unlock(&(connection->mutex_cond));

    g_free(data);
}


static gboolean
balde_incoming_callback(GThreadedSocketService *service,
    GSocketConnection *connection, GObject *source_object, gpointer user_data)
{
    GError *error = NULL;

    GInputStream *istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    GOutputStream *ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));

    balde_sapi_fcgi_user_data_t *ud = user_data;

    GHashTable *requests = g_hash_table_new_full(g_direct_hash, g_direct_equal,
        NULL, (GDestroyNotify) balde_sapi_fcgi_request_free);

    balde_sapi_fcgi_connection_t *conn = g_new(balde_sapi_fcgi_connection_t, 1);
    conn->requests = requests;
    conn->ostream = ostream;
    g_mutex_init(&(conn->mutex));
    g_mutex_init(&(conn->mutex_cond));
    g_cond_init(&(conn->cond));

    balde_sapi_fcgi_record_t *record = NULL;
    balde_sapi_fcgi_request_t *request = NULL;
    balde_sapi_fcgi_begin_request_body_t *brb = NULL;

    while (TRUE) {

        record = g_new(balde_sapi_fcgi_record_t, 1);

        record->header = balde_sapi_fcgi_read_header(istream);
        if (record->header == NULL) {
            goto cleanup;
        }

        gsize count = record->header->content_length + record->header->padding_length;
        gsize read;
        if (!g_input_stream_read_all(istream, record->buffer, count, &read, NULL, NULL)) {
            g_printerr("fcgi: error: failed to read FastCGI data, dropping connection.\n");
            goto cleanup;
        }

        if (count != read) {
            g_printerr("fcgi: error: failed to read FastCGI data. invalid data size, "
                "dropping connection.\n");
            goto cleanup;
        }

        g_mutex_lock(&(conn->mutex));
        request = g_hash_table_lookup(requests,
            GINT_TO_POINTER(record->header->request_id));
        g_mutex_unlock(&(conn->mutex));

        if ((request == NULL) &&
            (record->header->type != FCGI_BEGIN_REQUEST) &&
            (record->header->type != FCGI_GET_VALUES))
        {
            g_printerr("fcgi: error: unexpected FastCGI record type %d, dropping "
                "connection.\n", record->header->type);
            goto cleanup;
        }

        guint8 keep_conn = 1;

        switch ((balde_sapi_fcgi_record_type_t) record->header->type) {

            case FCGI_BEGIN_REQUEST:
                if (request != NULL) {
                    g_printerr("fcgi: error: unexpected FastCGI record type "
                        "BEGIN_REQUEST during request %d, dropping connection.\n",
                        request->id);
                    goto cleanup;
                }

                if (record->header->content_length != 8) {
                    g_printerr("fcgi: error: invalid data size for FastCGI "
                        "record type BEGIN_REQUEST: %d, dropping connection.\n",
                        record->header->content_length);
                    goto cleanup;
                }

                brb = (balde_sapi_fcgi_begin_request_body_t*) record->buffer;
                switch (g_ntohs(brb->role)) {

                    case FCGI_RESPONDER:
                        balde_sapi_fcgi_request_free(request);
                        request = g_new(balde_sapi_fcgi_request_t, 1);
                        request->id = record->header->request_id;
                        request->flags = brb->flags;
                        request->params = g_byte_array_new();
                        request->body = g_byte_array_new();
                        request->processed = FALSE;

                        g_mutex_lock(&(conn->mutex));
                        g_hash_table_insert(requests,
                            GINT_TO_POINTER(record->header->request_id),
                            request);
                        g_mutex_unlock(&(conn->mutex));
                        break;

                    // the following roles are not supported.
                    case FCGI_AUTHORIZER:
                    case FCGI_FILTER:
                        g_mutex_lock(&(conn->mutex));
                        balde_sapi_fcgi_send_eof(ostream, NULL, request->id,
                            FCGI_UNKNOWN_ROLE);
                        g_mutex_unlock(&(conn->mutex));
                        goto cleanup;

                }
                break;

            case FCGI_PARAMS:
                g_byte_array_append(request->params, record->buffer,
                    record->header->content_length);
                break;

            case FCGI_STDIN:
                if (record->header->content_length > 0) {
                    g_byte_array_append(request->body, record->buffer,
                        record->header->content_length);
                }
                else {
                    balde_sapi_fcgi_thread_user_data_t *tud =
                        g_new(balde_sapi_fcgi_thread_user_data_t, 1);
                    tud->connection = conn;
                    tud->request = request;
                    keep_conn &= request->flags;
                    g_thread_pool_push(ud->pool, tud, NULL);
                    if (keep_conn == 0) {
                        g_mutex_lock(&(conn->mutex_cond));
                        while (!request->processed)
                            g_cond_wait(&(conn->cond), &(conn->mutex_cond));
                        g_mutex_unlock(&(conn->mutex_cond));
                        request = NULL;
                        goto cleanup;
                    }
                    request = NULL;
                }
                break;

            case FCGI_GET_VALUES:
                g_mutex_lock(&(conn->mutex));
                balde_sapi_fcgi_send_get_values_result(ostream);
                g_mutex_unlock(&(conn->mutex));
                goto cleanup;

            case FCGI_DATA:
                goto cleanup;

            case FCGI_ABORT_REQUEST:
                // we don't support aborting requests from the webserver
                // because this can't be implemented for the other SAPIs.

            default:
                g_mutex_lock(&(conn->mutex));
                balde_sapi_fcgi_send_unknown_type(ostream, record->header->type);
                g_mutex_unlock(&(conn->mutex));
                goto cleanup;
        }

        balde_sapi_fcgi_record_free(record);
        record = NULL;
    }

cleanup:

    balde_sapi_fcgi_request_free(request);
    balde_sapi_fcgi_record_free(record);

    // this is a hack, to wait on the locks to be released before trying to
    // release resources.
    g_mutex_lock(&(conn->mutex_cond));
    g_mutex_unlock(&(conn->mutex_cond));
    g_mutex_clear(&(conn->mutex_cond));
    g_mutex_lock(&(conn->mutex));
    g_mutex_unlock(&(conn->mutex));
    g_mutex_clear(&(conn->mutex));

    g_cond_clear(&(conn->cond));
    g_free(conn);

    g_io_stream_close(G_IO_STREAM(connection), NULL, &error);
    if (error != NULL) {
        g_printerr("Failed to close connection: %s\n", error->message);
        g_error_free(error);
    }

    return TRUE;
}


static gboolean runfcgi = FALSE;
static gchar *host = NULL;
static gchar *socket = NULL;
static gint port = 9000;
static gint max_threads_server = 10;
static gint max_threads_app = 10;

static GOptionEntry entries_fcgi[] =
{
    {"runfcgi", 'f', 0, G_OPTION_ARG_NONE, &runfcgi, "Listen to FastCGI socket.",
        NULL},
    {"fcgi-host", 0, 0, G_OPTION_ARG_STRING, &host,
        "Embedded FastCGI server host. (default: 127.0.0.1)", "HOST"},
    {"fcgi-port", 0, 0, G_OPTION_ARG_INT, &port,
        "Embedded FastCGI server port. (default: 9000)", "PORT"},
    #ifdef WITH_UNIX_SOCKETS
    {"fcgi-socket", 0, 0, G_OPTION_ARG_STRING, &socket,
        "Embedded FastCGI server unix socket.", "SOCKET"},
    #endif
    {"fcgi-max-threads-server", 0, 0, G_OPTION_ARG_INT, &max_threads_server,
        "Embedded FastCGI max server threads. (default: 10)", "THREADS"},
    {"fcgi-max-threads-app", 0, 0, G_OPTION_ARG_INT, &max_threads_app,
        "Embedded FastCGI max application threads. (default: 10)", "THREADS"},
    {NULL}
};


static GOptionGroup*
balde_sapi_fcgi_init(void)
{
    GOptionGroup *fcgi_group = g_option_group_new("fcgi", "FastCGI Options:",
        "Show FastCGI help options", NULL, NULL);
    g_option_group_add_entries(fcgi_group, entries_fcgi);
    return fcgi_group;
}


static gboolean
balde_sapi_fcgi_supported(void)
{
    return runfcgi;
}

void
balde_cleanup_socket(gchar* socket, gboolean failed_to_run) {
    if (socket == NULL || failed_to_run == TRUE)
        return;
    GFile* file = g_file_new_for_path(socket);
    g_file_delete(file, NULL, NULL);
    g_object_unref(file);
}

static gint
balde_sapi_fcgi_run(balde_app_t *app)
{
    GError *error = NULL;
    const gchar *final_host = host != NULL ? host : "127.0.0.1";
    if (socket == NULL)
        g_printerr(" * Running FastCGI on %s:%d",
            final_host, port);
    else
        g_printerr(" * Running FastCGI on socket %s", socket);

    g_printerr(" (server threads: %d, app threads: %d)\n",
        max_threads_server, max_threads_app);
    GSocketService *service = g_threaded_socket_service_new(max_threads_server);
    GSocketAddress *address = NULL;
    GSocketProtocol protocol = G_SOCKET_PROTOCOL_TCP;
    #ifdef WITH_UNIX_SOCKETS
    if (socket == NULL)
        address = g_inet_socket_address_new_from_string(final_host, port);
    else {
        address = g_unix_socket_address_new(socket);
        protocol = G_SOCKET_PROTOCOL_DEFAULT;
    }
    #else
        address = g_inet_socket_address_new_from_string(final_host, port);
    #endif

    g_socket_listener_add_address(G_SOCKET_LISTENER(service), address,
        G_SOCKET_TYPE_STREAM, protocol, NULL, NULL, &error);
    if (error != NULL) {
        g_printerr("Failed to listen: %s\n", error->message);
        g_error_free(error);
        g_object_unref(service);
        g_object_unref(address);
        balde_cleanup_socket(socket, TRUE);
        g_free(host);
        return 3;
    }

    balde_sapi_fcgi_user_data_t *ud = g_new(balde_sapi_fcgi_user_data_t, 1);
    ud->app = app;
    ud->pool = g_thread_pool_new((GFunc) balde_handle_request, app,
        max_threads_app, FALSE, &error);
    if (error != NULL) {
        g_printerr("Failed to create app thread pool: %s\n", error->message);
        g_error_free(error);
        g_object_unref(service);
        g_object_unref(address);
        balde_cleanup_socket(socket, FALSE);
        g_free(host);
        g_free(ud);
        return 3;
    }

    g_signal_connect(service, "run", G_CALLBACK(balde_incoming_callback), ud);
    g_socket_service_start(service);
    g_object_unref(service);
    g_object_unref(address);
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
    g_thread_pool_free(ud->pool, TRUE, TRUE);
    g_free(ud);
    g_free(host);
    balde_cleanup_socket(socket, FALSE);
    return 0;
}


balde_sapi_t fcgi_sapi = {
    .name = "fcgi",
    .init = balde_sapi_fcgi_init,
    .supported = balde_sapi_fcgi_supported,
    .run = balde_sapi_fcgi_run,
};
