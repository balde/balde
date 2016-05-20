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
#include <glib/gstdio.h>
#include "../src/balde.h"
#include "../src/app.h"
#include "utils.h"


void
test_make_response(void)
{
    balde_response_t *res = balde_make_response("lol");
    g_assert(res != NULL);
    g_assert(res->status_code == 200);
    g_assert(g_hash_table_size(res->priv->headers) == 0);
    g_assert(g_hash_table_size(res->priv->template_ctx) == 0);
    g_assert_cmpstr(res->priv->body->str, ==, "lol");
    balde_response_free(res);
}


void
test_make_response_len(void)
{
    balde_response_t *res = balde_make_response_len("lolasdf", 3);
    g_assert(res != NULL);
    g_assert(res->status_code == 200);
    g_assert(g_hash_table_size(res->priv->headers) == 0);
    g_assert(g_hash_table_size(res->priv->template_ctx) == 0);
    g_assert_cmpstr(res->priv->body->str, ==, "lol");
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
    g_assert(g_hash_table_size(res->priv->headers) == 1);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "content-type");
    g_assert_cmpstr(tmp->data, ==, "text/plain; charset=utf-8");
    g_assert_cmpstr(res->priv->body->str, ==,
        "404 Not Found\n\nThe requested URL was not found on the server. "
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
    g_assert(g_hash_table_size(res->priv->headers) == 1);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "content-type");
    g_assert_cmpstr(tmp->data, ==, "text/plain; charset=utf-8");
    g_assert_cmpstr(res->priv->body->str, ==,
        "500 Internal Server Error\n\nThe server encountered an internal error "
        "and was unable to complete your request. Either the server is "
        "overloaded or there is an error in the application.\n\n(null)\n");
    balde_response_free(res);
    balde_app_free(app);
}


void
test_response_set_headers(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_header(res, "AsDf-QwEr", "test");
    g_assert(g_hash_table_size(res->priv->headers) == 1);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "asdf-qwer");
    g_assert_cmpstr(tmp->data, ==, "test");
    balde_response_free(res);
}


void
test_response_append_body(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_append_body(res, "hehe");
    g_assert_cmpstr(res->priv->body->str, ==, "lolhehe");
    balde_response_free(res);
}


void
test_response_append_body_len(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_append_body_len(res, "heheasd", 4);
    g_assert_cmpstr(res->priv->body->str, ==, "lolhehe");
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
test_response_set_tmpl_var(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_tmpl_var(res, "bola", "guda");
    g_assert(g_hash_table_size(res->priv->template_ctx) == 1);
    g_assert_cmpstr(g_hash_table_lookup(res->priv->template_ctx, "bola"), ==, "guda");
    balde_response_free(res);
}


void
test_response_get_tmpl_var(void)
{
    balde_response_t *res = balde_make_response("lol");
    g_hash_table_insert(res->priv->template_ctx, g_strdup("bola"), g_strdup("guda"));
    g_assert_cmpstr(balde_response_get_tmpl_var(res, "bola"), ==, "guda");
    balde_response_free(res);
}


void
test_response_set_cookie(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", -1, -1, NULL, NULL, FALSE,
        FALSE);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==, "bola=\"guda\"; Path=/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_expires(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", -1, 1234567890, NULL, NULL,
        FALSE, FALSE);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==,
        "bola=\"guda\"; Expires=Fri, 13-Feb-2009 23:31:30 GMT; Path=/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_max_age(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", 60, -1, NULL, NULL, FALSE,
        FALSE);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==,
        "bola=\"guda\"; Expires=Fri, 13-Feb-2009 23:32:30 GMT; Max-Age=60; Path=/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_expires_and_max_age(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", 60, 1235555555, NULL, NULL,
            FALSE, FALSE);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==,
        "bola=\"guda\"; Expires=Wed, 25-Feb-2009 09:52:35 GMT; Max-Age=60; Path=/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_path(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", -1, -1, "/bola/", NULL,
        FALSE, FALSE);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==, "bola=\"guda\"; Path=/bola/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_domain(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", -1, -1, NULL, "bola.com",
        FALSE, FALSE);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==, "bola=\"guda\"; Domain=\"bola.com\"; Path=/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_secure(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", -1, -1, NULL, NULL, TRUE,
        FALSE);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==, "bola=\"guda\"; Secure; Path=/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_httponly(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", -1, -1, NULL, NULL, FALSE,
        TRUE);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==, "bola=\"guda\"; HttpOnly; Path=/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_secure_and_httponly(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", -1, -1, NULL, NULL, TRUE,
        TRUE);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==, "bola=\"guda\"; Secure; HttpOnly; Path=/");
    balde_response_free(res);
}


void
test_response_delete_cookie(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_delete_cookie(res, "bola", NULL, NULL);
    GSList *tmp = g_hash_table_lookup(res->priv->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==,
        "bola=\"\"; Expires=Thu, 01-Jan-1970 00:00:00 GMT; Max-Age=0; Path=/");
    balde_response_free(res);
}


void
test_response_render(void)
{
    balde_response_t *res = balde_make_response("lol");
    GString *out = balde_response_render(res, TRUE);
    g_assert_cmpstr(out->str, ==,
        "Content-Type: text/html; charset=utf-8\r\nContent-Length: 3\r\n\r\nlol");
    g_string_free(out, TRUE);
    balde_response_free(res);
}


void
test_balde_response_generate_etag(void)
{
    balde_response_t *res = balde_make_response("quico");
    gchar *hash = balde_response_generate_etag(res, FALSE);
    g_assert_cmpstr("\"15929f6ea6e9a8e093b05cf723d1e424\"", ==, hash);
    g_free(hash);
    hash = balde_response_generate_etag(res, TRUE);
    g_assert_cmpstr("W/\"15929f6ea6e9a8e093b05cf723d1e424\"", ==, hash);
    g_free(hash);
    balde_response_free(res);
}


void
test_balde_response_add_etag(void)
{
    balde_response_t *res = balde_make_response("quico");
    balde_response_set_etag_header(res, FALSE);
    GSList *etag = g_hash_table_lookup(res->priv->headers, "etag");
    g_assert(etag != NULL);
    g_assert_cmpstr("\"15929f6ea6e9a8e093b05cf723d1e424\"", ==, etag->data);
    balde_response_free(res);

    res = balde_make_response("quico");
    balde_response_set_etag_header(res, TRUE);
    etag = g_hash_table_lookup(res->priv->headers, "etag");
    g_assert(etag != NULL);
    g_assert_cmpstr("W/\"15929f6ea6e9a8e093b05cf723d1e424\"", ==, etag->data);
    balde_response_free(res);
}


void
test_balde_response_etag_matching(void)
{
    g_setenv("HTTP_IF_NONE_MATCH", "\"15929f6ea6e9a8e093b05cf723d1e424\"",
        TRUE);
    g_setenv("PATH_INFO", "/", TRUE);
    g_setenv("REQUEST_METHOD", "GET", TRUE);
    g_setenv("SERVER_NAME", "bola", TRUE);

    balde_app_t *app = balde_app_init();

    balde_request_t *req = balde_make_request(app, NULL);
    balde_response_t *res = balde_make_response("quico");
    balde_response_etag_matching(req, res);
    g_assert_cmpstr("", ==, res->priv->body->str);
    g_assert_cmpint(304, ==, res->status_code);

    balde_request_free(req);
    balde_response_free(res);

    g_setenv("HTTP_IF_NONE_MATCH", "W/\"15929f6ea6e9a8e093b05cf723d1e424\"",
        TRUE);
    balde_request_t *req2 = balde_make_request(app, NULL);
    balde_response_t *res2 = balde_make_response("quico");
    balde_response_etag_matching(req2, res2);
    g_assert_cmpstr("", ==, res2->priv->body->str);
    g_assert_cmpint(304, ==, res2->status_code);

    balde_request_free(req2);
    balde_response_free(res2);

    balde_app_free(app);
}


void test_balde_response_truncate_body(void)
{
    balde_response_t *res = balde_make_response("quico");
    balde_response_truncate_body(res);
    g_assert_cmpstr("", ==, res->priv->body->str);
    balde_response_free(res);
}


void
test_response_render_with_custom_mime_type(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_header(res, "content-type", "text/plain");
    GString *out = balde_response_render(res, TRUE);
    g_assert_cmpstr(out->str, ==,
        "Content-Type: text/plain\r\nContent-Length: 3\r\n\r\nlol");
    g_string_free(out, TRUE);
    balde_response_free(res);
}


void
test_response_render_with_multiple_cookies(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", 60, -1, NULL, NULL, FALSE, FALSE);
    balde_response_set_cookie(res, "asd", "qwe", -1, -1, NULL, NULL, FALSE, TRUE);
    balde_response_set_cookie(res, "xd", ":D", -1, -1, "/bola/", NULL, TRUE, FALSE);
    GString *out = balde_response_render(res, TRUE);
    g_assert_cmpstr(out->str, ==,
        "Set-Cookie: bola=\"guda\"; Expires=Fri, 13-Feb-2009 23:32:30 GMT; Max-Age=60; Path=/\r\n"
        "Set-Cookie: asd=\"qwe\"; HttpOnly; Path=/\r\n"
        "Set-Cookie: xd=\":D\"; Secure; Path=/bola/\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: 3\r\n"
        "\r\n"
        "lol");
    g_string_free(out, TRUE);
    balde_response_free(res);
}


void
test_response_render_without_body(void)
{
    balde_response_t *res = balde_make_response("lol");
    GString *out = balde_response_render(res, FALSE);
    g_assert_cmpstr(out->str, ==,
        "Content-Type: text/html; charset=utf-8\r\nContent-Length: 3\r\n\r\n");
    g_string_free(out, TRUE);
    balde_response_free(res);
}


void
test_response_render_exception(void)
{
    balde_app_t *app = balde_app_init();
    balde_abort_set_error(app, 404);
    balde_response_t *res = balde_make_response_from_exception(app->error);
    g_assert(res != NULL);
    GString *out = balde_response_render(res, TRUE);
    g_assert_cmpstr(out->str, ==,
        "Status: 404 NOT FOUND\r\n"
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
test_response_render_exception_without_body(void)
{
    balde_app_t *app = balde_app_init();
    balde_abort_set_error(app, 404);
    balde_response_t *res = balde_make_response_from_exception(app->error);
    g_assert(res != NULL);
    GString *out = balde_response_render(res, FALSE);
    g_assert_cmpstr(out->str, ==,
        "Status: 404 NOT FOUND\r\n"
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
    g_test_add_func("/responses/make_response", test_make_response);
    g_test_add_func("/responses/make_response_len", test_make_response_len);
    g_test_add_func("/responses/make_response_from_exception",
        test_make_response_from_exception);
    g_test_add_func("/responses/make_response_from_exception_not_found",
        test_make_response_from_exception_not_found);
    g_test_add_func("/responses/make_response_from_external_exception",
        test_make_response_from_external_exception);
    g_test_add_func("/responses/set_headers", test_response_set_headers);
    g_test_add_func("/responses/append_body", test_response_append_body);
    g_test_add_func("/responses/append_body_len",
        test_response_append_body_len);
    g_test_add_func("/responses/fix_header_name", test_fix_header_name);
    g_test_add_func("/responses/set_tmpl_var", test_response_set_tmpl_var);
    g_test_add_func("/responses/get_tmpl_var", test_response_get_tmpl_var);
    g_test_add_func("/responses/set_cookie", test_response_set_cookie);
    g_test_add_func("/responses/set_cookie_with_expires",
        test_response_set_cookie_with_expires);
    g_test_add_func("/responses/set_cookie_with_max_age",
        test_response_set_cookie_with_max_age);
    g_test_add_func("/responses/set_cookie_with_expires_and_max_age",
        test_response_set_cookie_with_expires_and_max_age);
    g_test_add_func("/responses/set_cookie_with_path",
        test_response_set_cookie_with_path);
    g_test_add_func("/responses/set_cookie_with_domain",
        test_response_set_cookie_with_domain);
    g_test_add_func("/responses/set_cookie_with_secure",
        test_response_set_cookie_with_secure);
    g_test_add_func("/responses/set_cookie_with_httponly",
        test_response_set_cookie_with_httponly);
    g_test_add_func("/responses/set_cookie_with_secure_and_httponly",
        test_response_set_cookie_with_secure_and_httponly);
    g_test_add_func("/responses/delete_cookie", test_response_delete_cookie);
    g_test_add_func("/responses/generate_etag",
        test_balde_response_generate_etag);
    g_test_add_func("/responses/add_etag", test_balde_response_add_etag);
    g_test_add_func("/responses/etag_matching",
        test_balde_response_etag_matching);
    g_test_add_func("/responses/truncate_body",
        test_balde_response_truncate_body);
    g_test_add_func("/responses/render", test_response_render);
    g_test_add_func("/responses/render_with_custom_mime_type",
        test_response_render_with_custom_mime_type);
    g_test_add_func("/responses/render_with_multiple_cookies",
        test_response_render_with_multiple_cookies);
    g_test_add_func("/responses/render_without_body",
        test_response_render_without_body);
    g_test_add_func("/responses/render_exception",
        test_response_render_exception);
    g_test_add_func("/responses/render_exception_without_body",
        test_response_render_exception_without_body);
    return g_test_run();
}
