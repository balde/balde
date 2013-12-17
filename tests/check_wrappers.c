/**
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <balde/app.h>
#include <balde/exceptions.h>
#include <balde/wrappers.h>


void
test_make_response(void)
{
    balde_response_t *res = balde_make_response("lol");
    g_assert(res != NULL);
    g_assert(res->status_code == 200);
    g_assert(g_hash_table_size(res->headers) == 1);
    g_assert_cmpstr(g_hash_table_lookup(res->headers, "content-type"), ==,
        "text/html; charset=utf-8");
    g_assert_cmpstr(res->body->str, ==, "lol");
    balde_response_free(res);
}


void
test_make_response_from_exception(void)
{
    balde_app_t *app = balde_app_init();
    balde_abort_set_error(app, 404);
    balde_response_t *res = balde_make_response_from_exception(app->error);
    g_assert(res != NULL);
    g_assert(res->status_code == 404);
    g_assert(g_hash_table_size(res->headers) == 1);
    g_assert_cmpstr(g_hash_table_lookup(res->headers, "content-type"), ==,
        "text/plain; charset=utf-8");
    g_assert_cmpstr(res->body->str, ==,
        "Error: 404 Not Found\n\nThe requested URL was not found on the server. "
        "If you entered the URL manually please check your spelling and try again.\n");
    balde_response_free(res);
    balde_app_free(app);
}


void
test_make_response_from_exception_not_found(void)
{
    balde_response_t *res = balde_make_response_from_exception(NULL);
    g_assert(res == NULL);
}


void
test_make_response_from_external_exception(void)
{
    balde_app_t *app = balde_app_init();
    balde_abort_set_error(app, 1024);
    balde_response_t *res = balde_make_response_from_exception(app->error);
    g_assert(res != NULL);
    g_assert(res->status_code == 500);
    g_assert(g_hash_table_size(res->headers) == 1);
    g_assert_cmpstr(g_hash_table_lookup(res->headers, "content-type"), ==,
        "text/plain; charset=utf-8");
    g_assert_cmpstr(res->body->str, ==,
        "Error: 500 Internal Server Error\n\nThe server encountered an internal "
        "error and was unable to complete your request. Either the server is "
        "overloaded or there is an error in the application.\n\n1024 (null): (null)\n");
    balde_response_free(res);
    balde_app_free(app);
}


void
test_response_set_headers(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_header(res, "AsDf-QwEr", "test");
    g_assert(g_hash_table_size(res->headers) == 2);
    g_assert_cmpstr(g_hash_table_lookup(res->headers, "asdf-qwer"), ==, "test");
    balde_response_free(res);
}


void
test_response_append_body(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_append_body(res, "hehe");
    g_assert_cmpstr(res->body->str, ==, "lolhehe");
    balde_response_free(res);
}


void
test_fix_header_name(void)
{
    gchar foo[] = "content-type-lol";
    balde_fix_header_name(foo);
    g_assert_cmpstr(foo, ==, "Content-Type-Lol");
}


void
test_response_render(void)
{
    balde_response_t *res = balde_make_response("lol");
    gchar *out = balde_response_render(res);
    g_assert_cmpstr(out, ==,
        "Content-Type: text/html; charset=utf-8\r\nContent-Length: 3\r\n\r\nlol");
    g_free(out);
    balde_response_free(res);
}


void
test_response_render_exception(void)
{
    balde_app_t *app = balde_app_init();
    balde_abort_set_error(app, 404);
    balde_response_t *res = balde_make_response_from_exception(app->error);
    g_assert(res != NULL);
    gchar *out = balde_response_render(res);
    g_assert_cmpstr(out, ==,
        "Status: 404 Not Found\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: 143\r\n"
        "\r\n"
        "Error: 404 Not Found\n\nThe requested URL was not found on the server. "
        "If you entered the URL manually please check your spelling and try again.\n");
    g_free(out);
    balde_response_free(res);
    balde_app_free(app);
}


void
test_request_headers(void)
{
    g_setenv("HTTP_LOL_HEHE", "12345", TRUE);
    g_setenv("HTTP_XD_KKK", "asdf", TRUE);
    // FIXME: this thing is too weak :(
    GHashTable *headers = balde_request_headers();
    g_assert(g_hash_table_size(headers) == 2);
    g_assert_cmpstr(g_hash_table_lookup(headers, "lol-hehe"), ==, "12345");
    g_assert_cmpstr(g_hash_table_lookup(headers, "xd-kkk"), ==, "asdf");
    g_hash_table_destroy(headers);
}


void
test_make_request(void)
{
    g_setenv("HTTP_LOL_HEHE", "12345", TRUE);
    g_setenv("HTTP_XD_KKK", "asdf", TRUE);
    g_setenv("PATH_INFO", "/", TRUE);
    // FIXME: this thing is too weak :(
    balde_request_t *request = balde_make_request();
    g_assert_cmpstr(request->path, ==, "/");
    g_assert(g_hash_table_size(request->headers) == 2);
    g_assert(request->view_args == NULL);
    balde_request_free(request);
}


void
test_request_get_header(void)
{
    g_setenv("HTTP_LOL_HEHE", "12345", TRUE);
    g_setenv("HTTP_XD_KKK", "asdf", TRUE);
    // FIXME: this thing is too weak :(
    balde_request_t *request = balde_make_request();
    g_assert_cmpstr(balde_request_get_header(request, "Lol-Hehe"), ==, "12345");
    g_assert_cmpstr(balde_request_get_header(request, "XD-KKK"), ==, "asdf");
    g_assert(balde_request_get_header(request, "foo") == NULL);
    balde_request_free(request);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/wrappers/make_response", test_make_response);
    g_test_add_func("/wrappers/make_response_from_exception",
        test_make_response_from_exception);
    g_test_add_func("/wrappers/make_response_from_exception_not_found",
        test_make_response_from_exception_not_found);
    g_test_add_func("/wrappers/make_response_from_external_exception",
        test_make_response_from_external_exception);
    g_test_add_func("/wrappers/response_set_headers",
        test_response_set_headers);
    g_test_add_func("/wrappers/response_append_body",
        test_response_append_body);
    g_test_add_func("/wrappers/fix_header_name", test_fix_header_name);
    g_test_add_func("/wrappers/response_render", test_response_render);
    g_test_add_func("/wrappers/response_render_exception",
        test_response_render_exception);
    g_test_add_func("/wrappers/request_headers", test_request_headers);
    g_test_add_func("/wrappers/make_request", test_make_request);
    g_test_add_func("/wrappers/request_get_header", test_request_get_header);
    return g_test_run();
}
