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
#include <string.h>
#include "../src/balde.h"
#include "../src/app.h"
#include "../src/httpd.h"
#include "../src/requests.h"
#include "../src/responses.h"


void
test_httpd_parse_request(void)
{
    balde_app_t *app = balde_app_init();
    const gchar *test =
        "GET /bola?foo=bar&baz=lol HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Location: /foo\r\n"
        "Chunda: rs\r\n"
        "Content-Length: 6\r\n"
        "\r\n"
        "XD=asd\r\n";
    GInputStream *tmp = g_memory_input_stream_new_from_data (test, strlen(test), NULL);
    balde_httpd_parser_data_t *data = balde_httpd_parse_request(app, tmp);
    balde_request_env_t *req = data->env;
    g_object_unref(tmp);
    g_assert(req != NULL);
    g_assert_cmpstr(req->request_method, ==, "GET");
    g_assert_cmpstr(req->path_info, ==, "/bola");
    g_assert_cmpstr(req->query_string, ==, "foo=bar&baz=lol");
    g_assert(req->headers != NULL);
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "host"), ==, "example.com");
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "location"), ==, "/foo");
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "chunda"), ==, "rs");
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "content-length"), ==, "6");
    g_assert_cmpstr(req->body->str, ==, "XD=asd");
    g_assert_cmpint(req->body->len, ==, 6);
    g_assert(!req->https);
    g_free(data->request_line);
    g_free(data);
    balde_request_env_free(req);
    balde_app_free(app);
}


void
test_httpd_parse_request_without_query_string(void)
{
    balde_app_t *app = balde_app_init();
    const gchar *test =
        "GET /bola HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Location: /foo\r\n"
        "Chunda: rs\r\n"
        "Content-Length: 6\r\n"
        "\r\n"
        "XD=asd\r\n";
    GInputStream *tmp = g_memory_input_stream_new_from_data (test, strlen(test), NULL);
    balde_httpd_parser_data_t *data = balde_httpd_parse_request(app, tmp);
    balde_request_env_t *req = data->env;
    g_object_unref(tmp);
    g_assert(req != NULL);
    g_assert_cmpstr(req->request_method, ==, "GET");
    g_assert_cmpstr(req->path_info, ==, "/bola");
    g_assert_cmpstr(req->query_string, ==, "");
    g_assert(req->headers != NULL);
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "host"), ==, "example.com");
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "location"), ==, "/foo");
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "chunda"), ==, "rs");
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "content-length"), ==, "6");
    g_assert_cmpstr(req->body->str, ==, "XD=asd");
    g_assert_cmpint(req->body->len, ==, 6);
    g_free(data->request_line);
    g_free(data);
    balde_request_env_free(req);
    balde_app_free(app);
}


void
test_httpd_response_render(void)
{
    balde_response_t *res = balde_make_response("lol");
    GString *out = balde_httpd_response_render(res, TRUE);
    g_assert_cmpstr(out->str, ==,
        "HTTP/1.0 200 OK\r\n"
        "Date: Fri, 13 Feb 2009 23:31:30 GMT\r\n"
        "Connection: close\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: 3\r\n"
        "\r\n"
        "lol");
    g_string_free(out, TRUE);
    balde_response_free(res);
}


void
test_httpd_response_render_with_custom_mime_type(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_header(res, "content-type", "text/plain");
    GString *out = balde_httpd_response_render(res, TRUE);
    g_assert_cmpstr(out->str, ==,
        "HTTP/1.0 200 OK\r\n"
        "Date: Fri, 13 Feb 2009 23:31:30 GMT\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 3\r\n"
        "\r\n"
        "lol");
    g_string_free(out, TRUE);
    balde_response_free(res);
}


void
test_httpd_response_render_with_multiple_cookies(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", 60, -1, NULL, NULL, FALSE, FALSE);
    balde_response_set_cookie(res, "asd", "qwe", -1, -1, NULL, NULL, FALSE, TRUE);
    balde_response_set_cookie(res, "xd", ":D", -1, -1, "/bola/", NULL, TRUE, FALSE);
    GString *out = balde_httpd_response_render(res, TRUE);
    g_assert_cmpstr(out->str, ==,
        "HTTP/1.0 200 OK\r\n"
        "Date: Fri, 13 Feb 2009 23:31:30 GMT\r\n"
        "Set-Cookie: bola=\"guda\"; Expires=Fri, 13-Feb-2009 23:32:30 GMT; Max-Age=60; Path=/\r\n"
        "Set-Cookie: asd=\"qwe\"; HttpOnly; Path=/\r\n"
        "Set-Cookie: xd=\":D\"; Secure; Path=/bola/\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: close\r\n"
        "Content-Length: 3\r\n"
        "\r\n"
        "lol");
    g_string_free(out, TRUE);
    balde_response_free(res);
}


void
test_httpd_response_render_without_body(void)
{
    balde_response_t *res = balde_make_response("lol");
    GString *out = balde_httpd_response_render(res, FALSE);
    g_assert_cmpstr(out->str, ==,
        "HTTP/1.0 200 OK\r\n"
        "Date: Fri, 13 Feb 2009 23:31:30 GMT\r\n"
        "Connection: close\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: 3\r\n"
        "\r\n");
    g_string_free(out, TRUE);
    balde_response_free(res);
}


void
test_httpd_response_render_exception(void)
{
    balde_app_t *app = balde_app_init();
    balde_abort_set_error(app, 404);
    balde_response_t *res = balde_make_response_from_exception(app->error);
    g_assert(res != NULL);
    GString *out = balde_httpd_response_render(res, TRUE);
    g_assert_cmpstr(out->str, ==,
        "HTTP/1.0 404 NOT FOUND\r\n"
        "Date: Fri, 13 Feb 2009 23:31:30 GMT\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: 136\r\n"
        "\r\n"
        "404 Not Found\n\nThe requested URL was not found on the server. "
        "If you entered the URL manually please check your spelling and try again.\n");
    g_string_free(out, TRUE);
    balde_response_free(res);
    balde_app_free(app);
}


void
test_httpd_response_render_exception_without_body(void)
{
    balde_app_t *app = balde_app_init();
    balde_abort_set_error(app, 404);
    balde_response_t *res = balde_make_response_from_exception(app->error);
    g_assert(res != NULL);
    GString *out = balde_httpd_response_render(res, FALSE);
    g_assert_cmpstr(out->str, ==,
        "HTTP/1.0 404 NOT FOUND\r\n"
        "Date: Fri, 13 Feb 2009 23:31:30 GMT\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: 136\r\n"
        "\r\n");
    g_string_free(out, TRUE);
    balde_response_free(res);
    balde_app_free(app);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/httpd/parse_request", test_httpd_parse_request);
    g_test_add_func("/httpd/parse_request_without_query_string",
        test_httpd_parse_request_without_query_string);
    g_test_add_func("/httpd/response_render", test_httpd_response_render);
    g_test_add_func("/httpd/response_render_with_custom_mime_type",
        test_httpd_response_render_with_custom_mime_type);
    g_test_add_func("/httpd/response_render_with_multiple_cookies",
        test_httpd_response_render_with_multiple_cookies);
    g_test_add_func("/httpd/response_render_without_body",
        test_httpd_response_render_without_body);
    g_test_add_func("/httpd/response_render_exception",
        test_httpd_response_render_exception);
    g_test_add_func("/httpd/response_render_exception_without_body",
        test_httpd_response_render_exception_without_body);
    return g_test_run();
}
