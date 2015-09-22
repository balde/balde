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
#include "../src/scgi.h"
#include "../src/requests.h"


void
test_scgi_parse_request(void)
{
    balde_app_t *app = balde_app_init();
    gchar *test = g_strdup_printf(
        "136:CONTENT_LENGTH%c6%c"          // 14 + 1 + 2 = 17
        "REQUEST_METHOD%cGET%c"            // 14 + 3 + 2 = 19
        "PATH_INFO%c/bola%c"               // 9 + 5 + 2 = 16
        "HTTP_HOST%cexample.com%c"         // 9 + 11 + 2 = 22
        "HTTP_LOCATION%c/foo%c"            // 13 + 4 + 2 = 19
        "QUERY_STRING%cfoo=bar&baz=lol%c"  // 12 + 15 + 2 = 29
        "HTTP_CHUNDA%crs,"                 // 11 + 2 + 1 = 14
        "XD=asd\r\n", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    GInputStream *tmp = g_memory_input_stream_new_from_data(test, 149, NULL);
    balde_request_env_t *req = balde_scgi_parse_request(app, tmp);
    g_object_unref(tmp);
    g_free(test);
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
    balde_request_env_free(req);
    balde_app_free(app);
}


void
test_scgi_parse_request_without_query_string(void)
{
    balde_app_t *app = balde_app_init();
    gchar *test = g_strdup_printf(
        "107:CONTENT_LENGTH%c6%c"   // 14 + 1 + 2 = 17
        "REQUEST_METHOD%cGET%c"     // 14 + 3 + 2 = 19
        "PATH_INFO%c/bola%c"        // 9 + 5 + 2 = 16
        "HTTP_HOST%cexample.com%c"  // 9 + 11 + 2 = 22
        "HTTP_LOCATION%c/foo%c"     // 13 + 4 + 2 = 19
        "HTTP_CHUNDA%crs,"          // 11 + 2 + 1 = 14
        "XD=asd\r\n", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    GInputStream *tmp = g_memory_input_stream_new_from_data(test, 120, NULL);
    balde_request_env_t *req = balde_scgi_parse_request(app, tmp);
    g_object_unref(tmp);
    g_free(test);
    g_assert(req != NULL);
    g_assert_cmpstr(req->request_method, ==, "GET");
    g_assert_cmpstr(req->path_info, ==, "/bola");
    g_assert(req->query_string == NULL);
    g_assert(req->headers != NULL);
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "host"), ==, "example.com");
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "location"), ==, "/foo");
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "chunda"), ==, "rs");
    g_assert_cmpstr(g_hash_table_lookup(req->headers, "content-length"), ==, "6");
    g_assert_cmpstr(req->body->str, ==, "XD=asd");
    g_assert_cmpint(req->body->len, ==, 6);
    balde_request_env_free(req);
    balde_app_free(app);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/scgi/parse_request", test_scgi_parse_request);
    g_test_add_func("/scgi/parse_request_without_query_string",
        test_scgi_parse_request_without_query_string);
    return g_test_run();
}
