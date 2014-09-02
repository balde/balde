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
#include <string.h>

#include <balde/app-private.h>
#include <balde/exceptions-private.h>
#include <balde/httpd-private.h>
#include <balde/wrappers-private.h>
#include "http-parser/http_parser.h"

#ifndef SOCKET_BUFFER_SIZE
#define SOCKET_BUFFER_SIZE 4096
#endif

static http_parser_settings settings;


static int
balde_httpd_url_cb(http_parser* parser, const char* chunk, size_t len)
{
    balde_httpd_parser_data_t *data = parser->data;
    struct http_parser_url u;
    gchar *url = g_strndup(chunk, len);
    http_parser_parse_url(url, len, 0, &u);
    if (u.field_set & (1 << UF_PATH))
        data->request->path_info = g_strndup(url + u.field_data[UF_PATH].off,
            u.field_data[UF_PATH].len);
    else
        data->request->path_info = g_strdup("/");
    if (u.field_set & (1 << UF_QUERY))
        data->request->query_string = g_strndup(url + u.field_data[UF_QUERY].off,
            u.field_data[UF_QUERY].len);
    else
        data->request->query_string = g_strdup("");
    g_free(url);
    return 0;
}


static int
balde_httpd_body_cb(http_parser* parser, const char* chunk, size_t len)
{
    balde_httpd_parser_data_t *data = parser->data;
    if (data->body == NULL)
        data->body = g_string_new("");
    g_string_append_len(data->body, chunk, len);
    return 0;
}


static int
balde_httpd_message_complete_cb(http_parser* parser)
{
    balde_httpd_parser_data_t *data = parser->data;
    data->request->content_length = 0;
    data->request->body = NULL;
    if (data->body != NULL) {
        data->request->content_length = data->body->len;
        data->request->body = g_string_free(data->body, FALSE);
    }
    return 0;
}


static int
balde_httpd_header_field_cb(http_parser* parser, const char* chunk, size_t len)
{
    balde_httpd_parser_data_t *data = parser->data;
    data->header_key = g_ascii_strdown(chunk, len);
    return 0;
}


static int
balde_httpd_header_value_cb(http_parser* parser, const char* chunk, size_t len)
{
    balde_httpd_parser_data_t *data = parser->data;
    g_hash_table_insert(data->request->headers, data->header_key,
        g_strndup(chunk, len));
    data->header_key = NULL;
    return 0;
}


static int
balde_httpd_headers_complete_cb(http_parser* parser) {
    balde_httpd_parser_data_t *data = parser->data;
    const char* method = http_method_str(parser->method);
    data->request->request_method = g_strndup(method, strlen(method));
    return 0;
}


balde_request_env_t*
balde_httpd_parse_request(GString *request)
{
    settings.on_url = balde_httpd_url_cb;
    settings.on_body = balde_httpd_body_cb;
    settings.on_header_field = balde_httpd_header_field_cb;
    settings.on_header_value = balde_httpd_header_value_cb;
    settings.on_message_complete = balde_httpd_message_complete_cb;
    settings.on_headers_complete = balde_httpd_headers_complete_cb;
    balde_httpd_parser_data_t *data = g_new(balde_httpd_parser_data_t, 1);
    data->body = NULL;
    data->header_key = NULL;
    data->request = g_new(balde_request_env_t, 1);
    data->request->path_info = NULL;
    data->request->request_method = NULL;
    data->request->query_string = NULL;
    data->request->headers = g_hash_table_new_full(g_str_hash, g_str_equal,
        g_free, g_free);
    data->request->content_length = 0;
    data->request->body = NULL;
    http_parser parser;
    parser.data = data;
    http_parser_init(&parser, HTTP_REQUEST);
    http_parser_execute(&parser, &settings, request->str, request->len);
    balde_request_env_t *rv = data->request;
    g_free(data->header_key);
    g_free(data);
    return rv;
}


GString*
balde_httpd_response_render(balde_response_t *response, const gboolean with_body)
{
    if (response == NULL)
        return NULL;
    GString *str = g_string_new("");
    g_string_append_printf(str, "HTTP/1.1 %d %s\r\n", response->status_code,
        balde_exception_get_name_from_code(response->status_code));
    gchar *len = g_strdup_printf("%zu", response->body->len);
    // FIXME: add date header
    balde_response_set_header(response, "Connection", "close");
    balde_response_set_header(response, "Content-Length", len);
    g_free(len);
    if (g_hash_table_lookup(response->headers, "content-type") == NULL)
        balde_response_set_header(response, "Content-Type", "text/html; charset=utf-8");
    g_hash_table_foreach(response->headers, (GHFunc) balde_header_render, str);
    g_string_append(str, "\r\n");
    if (with_body)
        g_string_append_len(str, response->body->str, response->body->len);
    return str;
}


static gboolean
balde_incoming_callback(GSocketService *service, GSocketConnection *connection,
    GObject *source_object, gpointer user_data)
{
    GError *error = NULL;
    GInputStream * istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    gchar message[SOCKET_BUFFER_SIZE];
    gssize message_len;
    GString *content = g_string_new("");
    do {
        message_len = g_input_stream_read(istream, message, SOCKET_BUFFER_SIZE,
            NULL, &error);
        if (error != NULL) {
            g_printerr("Failed to read: %s\n", error->message);
            g_error_free(error);
            return TRUE;
        }
        g_string_append_len(content, message, message_len);
    }
    while(message_len == SOCKET_BUFFER_SIZE || message_len == 0);
    balde_request_env_t *env = balde_httpd_parse_request(content);
    g_string_free(content, TRUE);
    balde_app_t *app = (balde_app_t*) user_data;
    GString *response = balde_app_main_loop(app, env, balde_httpd_response_render);
    GOutputStream *ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));
    g_output_stream_write(ostream, response->str, response->len, NULL, &error);
    g_string_free(response, TRUE);
    if (error != NULL) {
        g_printerr("Failed to send: %s\n", error->message);
        g_error_free(error);
        return TRUE;
    }
    return FALSE;
}


void
balde_httpd_run(balde_app_t *app, const gchar *host, gint16 port)
{
    GError *error = NULL;
    const gchar *final_host = host != NULL ? host : "127.0.0.1";
    g_printerr(" * Running on http://%s:%d/\n", final_host, port);
    GSocketService *service = g_socket_service_new();
    GInetAddress* addr_host = g_inet_address_new_from_string(final_host);
    GSocketAddress *address = g_inet_socket_address_new(addr_host, port);
    g_socket_listener_add_address((GSocketListener*) service, address,
        G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, NULL, NULL, &error);
    if (error != NULL) {
        g_printerr("Failed to listen: %s\n", error->message);
        g_error_free(error);
        g_object_unref(service);
        g_object_unref(addr_host);
        g_object_unref(address);
        return;
    }
    g_signal_connect(service, "incoming", G_CALLBACK(balde_incoming_callback), app);
    g_socket_service_start(service);
    g_object_unref(service);
    g_object_unref(addr_host);
    g_object_unref(address);
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
}
