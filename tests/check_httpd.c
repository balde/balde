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


void
test_httpd_parse_request(void)
{
    const gchar *test =
        "GET /bola?foo=bar&baz=lol HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Location: /foo\r\n"
        "Chunda: rs\r\n"
        "Content-Length: 6\r\n"
        "\r\n"
        "XD=asd\r\n";
    balde_request_env_t *req = balde_httpd_parse_request(test, strlen(test));
    g_assert(req != NULL);
    g_assert_cmpstr(req->request_method, ==, "GET");
    g_assert_cmpstr(req->path_info, ==, "/bola");
    g_assert_cmpstr(req->query_string, ==, "foo=bar&baz=lol");
    g_assert(req->headers != NULL);
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "host"), ==, "example.com");
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "location"), ==, "/foo");
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "chunda"), ==, "rs");
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "content-length"), ==, "6");
    g_assert_cmpint(req->content_length, ==, 6);
    g_assert_cmpstr(req->body, ==, "XD=asd");
    balde_request_env_free(req);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/httpd/parse_request", test_httpd_parse_request);
    return g_test_run();
}
