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
#include <string.h>

#include <balde/httpd-private.h>
#include <balde/wrappers-private.h>
#include "http-parser/http_parser.h"

static http_parser_settings settings;


static int
balde_httpd_url_cb(http_parser* parser, const char* chunk, size_t len)
{
    balde_httpd_parser_data_t *data = parser->data;
    struct http_parser_url u;
    gchar *url = g_strndup(chunk, len);
    http_parser_parse_url(url, len, 0, &u);
    data->request->path_info = g_strndup(url + u.field_data[UF_PATH].off,
        u.field_data[UF_PATH].len);
    data->request->query_string = g_strndup(url + u.field_data[UF_QUERY].off,
        u.field_data[UF_QUERY].len);
    data->request->script_name = g_strdup("");
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
    data->request->content_length = data->body->len;
    data->request->body = g_string_free(data->body, FALSE);
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
balde_httpd_parse_request(const gchar* request, gsize len)
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
    data->request->script_name = NULL;
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
    http_parser_execute(&parser, &settings, request, len);
    balde_request_env_t *rv = data->request;
    g_free(data->header_key);
    g_free(data);
    return rv;
}
