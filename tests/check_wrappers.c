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
#include <balde/app.h>
#include <balde/app-private.h>
#include <balde/exceptions.h>
#include <balde/wrappers.h>


void
test_make_response(void)
{
    balde_response_t *res = balde_make_response("lol");
    g_assert(res != NULL);
    g_assert(res->status_code == 200);
    g_assert(g_hash_table_size(res->headers) == 0);
    g_assert(g_hash_table_size(res->template_ctx) == 0);
    g_assert_cmpstr(res->body->str, ==, "lol");
    balde_response_free(res);
}


void
test_make_response_len(void)
{
    balde_response_t *res = balde_make_response_len("lolasdf", 3);
    g_assert(res != NULL);
    g_assert(res->status_code == 200);
    g_assert(g_hash_table_size(res->headers) == 0);
    g_assert(g_hash_table_size(res->template_ctx) == 0);
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
    GSList *tmp = g_hash_table_lookup(res->headers, "content-type");
    g_assert_cmpstr(tmp->data, ==, "text/plain; charset=utf-8");
    g_assert_cmpstr(res->body->str, ==,
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
    g_assert(g_hash_table_size(res->headers) == 1);
    GSList *tmp = g_hash_table_lookup(res->headers, "content-type");
    g_assert_cmpstr(tmp->data, ==, "text/plain; charset=utf-8");
    g_assert_cmpstr(res->body->str, ==,
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
    g_assert(g_hash_table_size(res->headers) == 1);
    GSList *tmp = g_hash_table_lookup(res->headers, "asdf-qwer");
    g_assert_cmpstr(tmp->data, ==, "test");
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
test_response_append_body_len(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_append_body_len(res, "heheasd", 4);
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
test_response_set_tmpl_var(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_tmpl_var(res, "bola", "guda");
    g_assert(g_hash_table_size(res->template_ctx) == 1);
    g_assert_cmpstr(g_hash_table_lookup(res->template_ctx, "bola"), ==, "guda");
    balde_response_free(res);
}


void
test_response_get_tmpl_var(void)
{
    balde_response_t *res = balde_make_response("lol");
    g_hash_table_insert(res->template_ctx, g_strdup("bola"), g_strdup("guda"));
    g_assert_cmpstr(balde_response_get_tmpl_var(res, "bola"), ==, "guda");
    balde_response_free(res);
}


void
test_response_set_cookie(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", -1, -1, NULL, NULL, FALSE);
    GSList *tmp = g_hash_table_lookup(res->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==, "bola=\"guda\"; Path=/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_expires(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", -1, 1234567890, NULL, NULL,
        FALSE);
    GSList *tmp = g_hash_table_lookup(res->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==,
        "bola=\"guda\"; Expires=Fri, 13-Feb-2009 23:31:30 GMT; Path=/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_max_age(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", 60, -1, NULL, NULL, FALSE);
    GSList *tmp = g_hash_table_lookup(res->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==,
        "bola=\"guda\"; Expires=Fri, 13-Feb-2009 23:32:30 GMT; Max-Age=60; Path=/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_expires_and_max_age(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", 60, 1235555555, NULL, NULL, FALSE);
    GSList *tmp = g_hash_table_lookup(res->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==,
        "bola=\"guda\"; Expires=Wed, 25-Feb-2009 09:52:35 GMT; Max-Age=60; Path=/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_path(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", -1, -1, "/bola/", NULL, FALSE);
    GSList *tmp = g_hash_table_lookup(res->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==, "bola=\"guda\"; Path=/bola/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_domain(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", -1, -1, NULL, "bola.com", FALSE);
    GSList *tmp = g_hash_table_lookup(res->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==, "bola=\"guda\"; Domain=\"bola.com\"; Path=/");
    balde_response_free(res);
}


void
test_response_set_cookie_with_secure(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_set_cookie(res, "bola", "guda", -1, -1, NULL, NULL, TRUE);
    GSList *tmp = g_hash_table_lookup(res->headers, "set-cookie");
    g_assert_cmpstr(tmp->data, ==, "bola=\"guda\"; Secure; Path=/");
    balde_response_free(res);
}


void
test_response_delete_cookie(void)
{
    balde_response_t *res = balde_make_response("lol");
    balde_response_delete_cookie(res, "bola", NULL, NULL);
    GSList *tmp = g_hash_table_lookup(res->headers, "set-cookie");
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
    balde_response_set_cookie(res, "bola", "guda", 60, -1, NULL, NULL, FALSE);
    balde_response_set_cookie(res, "asd", "qwe", -1, -1, NULL, NULL, FALSE);
    balde_response_set_cookie(res, "xd", ":D", -1, -1, "/bola/", NULL, FALSE);
    GString *out = balde_response_render(res, TRUE);
    g_assert_cmpstr(out->str, ==,
        "Set-Cookie: bola=\"guda\"; Expires=Fri, 13-Feb-2009 23:32:30 GMT; Max-Age=60; Path=/\r\n"
        "Set-Cookie: asd=\"qwe\"; Path=/\r\n"
        "Set-Cookie: xd=\":D\"; Path=/bola/\r\n"
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
        "Status: 404 Not Found\r\n"
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
        "Status: 404 Not Found\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: 136\r\n"
        "\r\n");
    g_string_free(out, TRUE);
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
test_urldecode(void)
{
    gchar *rv = balde_urldecode("saf%3Dgfd+123");
    g_assert_cmpstr(rv, ==, "saf=gfd 123");
    g_free(rv);
}


void
test_parse_query_string(void)
{
    GHashTable *qs = balde_parse_query_string("fdds%3Dfsd=etrh+adsf&asd+asd=vfdvf%3Dlol");
    g_assert(g_hash_table_size(qs) == 2);
    g_assert_cmpstr(g_hash_table_lookup(qs, "fdds=fsd"), ==, "etrh adsf");
    g_assert_cmpstr(g_hash_table_lookup(qs, "asd asd"), ==, "vfdvf=lol");
    g_hash_table_destroy(qs);
}


void
test_parse_cookies(void)
{
    GHashTable *c = balde_parse_cookies("bola=\"guda\"; guda=chunda");
    g_assert(g_hash_table_size(c) == 2);
    g_assert_cmpstr(g_hash_table_lookup(c, "bola"), ==, "guda");
    g_assert_cmpstr(g_hash_table_lookup(c, "guda"), ==, "chunda");
    g_hash_table_destroy(c);
}


void
test_make_request(void)
{
    g_setenv("HTTP_LOL_HEHE", "12345", TRUE);
    g_setenv("HTTP_XD_KKK", "asdf", TRUE);
    g_setenv("HTTP_COOKIE", "asd=\"qwe\"; bola=guda", TRUE);
    g_setenv("PATH_INFO", "/", TRUE);
    g_setenv("REQUEST_METHOD", "GET", TRUE);
    g_setenv("QUERY_STRING", "asd=lol&xd=hehe", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app);
    g_assert_cmpstr(request->path, ==, "/");
    g_assert(request->method == BALDE_HTTP_GET);
    g_assert(g_hash_table_size(request->headers) == 3);
    g_assert(g_hash_table_size(request->args) == 2);
    g_assert(g_hash_table_size(request->cookies) == 2);
    g_assert(request->view_args == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_request_get_header(void)
{
    g_setenv("HTTP_LOL_HEHE", "12345", TRUE);
    g_setenv("HTTP_XD_KKK", "asdf", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app);
    g_assert_cmpstr(balde_request_get_header(request, "Lol-Hehe"), ==, "12345");
    g_assert_cmpstr(balde_request_get_header(request, "XD-KKK"), ==, "asdf");
    g_assert(balde_request_get_header(request, "foo") == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_request_get_arg(void)
{
    g_setenv("QUERY_STRING", "lol=hehe", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app);
    g_assert_cmpstr(balde_request_get_arg(request, "lol"), == , "hehe");
    g_assert(balde_request_get_header(request, "xd") == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_request_get_form(void)
{
    g_setenv("REQUEST_METHOD", "GET", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app);
    g_assert(request->stream == NULL);
    g_assert(request->form == NULL);
    g_assert(balde_request_get_form(request, "lol") == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_request_get_form_with_empty_body(void)
{
    g_setenv("REQUEST_METHOD", "POST", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    // ommited CONTENT_LENGTH
    balde_request_t *request = balde_make_request(app);
    g_assert(request->stream == NULL);
    g_assert(g_hash_table_size(request->form) == 0);
    g_assert(balde_request_get_form(request, "lol") == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_request_get_view_arg(void)
{
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app);
    request->view_args = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_replace(request->view_args, g_strdup("foo"), g_strdup("bar"));
    g_assert_cmpstr(balde_request_get_view_arg(request, "foo"), == , "bar");
    g_assert(balde_request_get_view_arg(request, "xd") == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_request_get_cookie(void)
{
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app);
    g_hash_table_replace(request->cookies, g_strdup("foo"), g_strdup("bar"));
    g_assert_cmpstr(balde_request_get_cookie(request, "foo"), == , "bar");
    g_assert(balde_request_get_cookie(request, "xd") == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/wrappers/make_response", test_make_response);
    g_test_add_func("/wrappers/make_response_len", test_make_response_len);
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
    g_test_add_func("/wrappers/response_append_body_len",
        test_response_append_body_len);
    g_test_add_func("/wrappers/fix_header_name", test_fix_header_name);
    g_test_add_func("/wrappers/response_set_tmpl_var",
        test_response_set_tmpl_var);
    g_test_add_func("/wrappers/response_get_tmpl_var",
        test_response_get_tmpl_var);
    g_test_add_func("/wrappers/response_set_cookie",
        test_response_set_cookie);
    g_test_add_func("/wrappers/response_set_cookie_with_expires",
        test_response_set_cookie_with_expires);
    g_test_add_func("/wrappers/response_set_cookie_with_max_age",
        test_response_set_cookie_with_max_age);
    g_test_add_func("/wrappers/response_set_cookie_with_expires_and_max_age",
        test_response_set_cookie_with_expires_and_max_age);
    g_test_add_func("/wrappers/response_set_cookie_with_path",
        test_response_set_cookie_with_path);
    g_test_add_func("/wrappers/response_set_cookie_with_domain",
        test_response_set_cookie_with_domain);
    g_test_add_func("/wrappers/response_set_cookie_with_secure",
        test_response_set_cookie_with_secure);
    g_test_add_func("/wrappers/response_delete_cookie",
        test_response_delete_cookie);
    g_test_add_func("/wrappers/response_render", test_response_render);
    g_test_add_func("/wrappers/response_render_with_custom_mime_type",
        test_response_render_with_custom_mime_type);
    g_test_add_func("/wrappers/response_render_with_multiple_cookies",
        test_response_render_with_multiple_cookies);
    g_test_add_func("/wrappers/response_render_without_body",
        test_response_render_without_body);
    g_test_add_func("/wrappers/response_render_exception",
        test_response_render_exception);
    g_test_add_func("/wrappers/response_render_exception_without_body",
        test_response_render_exception_without_body);
    g_test_add_func("/wrappers/request_headers", test_request_headers);
    g_test_add_func("/wrappers/urldecode", test_urldecode);
    g_test_add_func("/wrappers/parse_query_string", test_parse_query_string);
    g_test_add_func("/wrappers/parse_cookies", test_parse_cookies);
    g_test_add_func("/wrappers/make_request", test_make_request);
    g_test_add_func("/wrappers/request_get_header", test_request_get_header);
    g_test_add_func("/wrappers/request_get_arg", test_request_get_arg);
    g_test_add_func("/wrappers/request_get_form", test_request_get_form);
    g_test_add_func("/wrappers/request_get_form_with_empty_body",
        test_request_get_form_with_empty_body);
    g_test_add_func("/wrappers/request_get_view_arg", test_request_get_view_arg);
    g_test_add_func("/wrappers/request_get_cookie", test_request_get_cookie);
    return g_test_run();
}
