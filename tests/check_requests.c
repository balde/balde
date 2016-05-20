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
test_parse_authorization(void)
{
    g_assert(balde_parse_authorization(NULL) == NULL);
    g_assert(balde_parse_authorization("") == NULL);
    g_assert(balde_parse_authorization("Bola afddsfsdfdsgfdg") == NULL);
    g_assert(balde_parse_authorization("Basic Ym9sYQ==") == NULL);  // bola
    balde_authorization_t *a = balde_parse_authorization("Basic Ym9sYTpndWRh");  // bola:guda
    g_assert(a != NULL);
    g_assert_cmpstr(a->username, ==, "bola");
    g_assert_cmpstr(a->password, ==, "guda");
    balde_authorization_free(a);
    a = balde_parse_authorization("Basic Ym9sYTo=");  // bola:
    g_assert(a != NULL);
    g_assert_cmpstr(a->username, ==, "bola");
    g_assert_cmpstr(a->password, ==, "");
    balde_authorization_free(a);
    a = balde_parse_authorization("Basic Ym9sYTpndWRhOmxvbA==");  // bola:guda:lol
    g_assert(a != NULL);
    g_assert_cmpstr(a->username, ==, "bola");
    g_assert_cmpstr(a->password, ==, "guda:lol");
    balde_authorization_free(a);
}


void
test_make_request(void)
{
    g_setenv("HTTP_LOL_HEHE", "12345", TRUE);
    g_setenv("HTTP_XD_KKK", "asdf", TRUE);
    g_setenv("HTTP_COOKIE", "asd=\"qwe\"; bola=guda", TRUE);
    g_setenv("HTTP_AUTHORIZATION", "Basic Ym9sYTpndWRhOmxvbA==", TRUE);
    g_setenv("PATH_INFO", "/", TRUE);
    g_setenv("REQUEST_METHOD", "GET", TRUE);
    g_setenv("QUERY_STRING", "asd=lol&xd=hehe", TRUE);
    g_setenv("SERVER_NAME", "bola", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    g_assert_cmpstr(request->path, ==, "/");
    g_assert_cmpstr(request->server_name, ==, "bola");
    g_assert(request->script_name == NULL);
    g_assert(request->method == BALDE_HTTP_GET);
    g_assert(g_hash_table_size(request->priv->headers) == 4);
    g_assert(g_hash_table_size(request->priv->args) == 2);
    g_assert(g_hash_table_size(request->priv->cookies) == 2);
    g_assert(request->authorization != NULL);
    g_assert_cmpstr(request->authorization->username, ==, "bola");
    g_assert_cmpstr(request->authorization->password, ==, "guda:lol");
    g_assert(request->priv->view_args == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_make_request_without_path_info(void)
{
    g_setenv("HTTP_LOL_HEHE", "12345", TRUE);
    g_setenv("HTTP_XD_KKK", "asdf", TRUE);
    g_setenv("HTTP_COOKIE", "asd=\"qwe\"; bola=guda", TRUE);
    g_setenv("HTTP_AUTHORIZATION", "Basic Ym9sYTpndWRhOmxvbA==", TRUE);
    g_setenv("REQUEST_METHOD", "GET", TRUE);
    g_setenv("QUERY_STRING", "asd=lol&xd=hehe", TRUE);
    g_unsetenv("PATH_INFO");
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    g_assert(request->path == NULL);
    g_assert(request->script_name == NULL);
    g_assert(request->method == BALDE_HTTP_GET);
    g_assert(g_hash_table_size(request->priv->headers) == 4);
    g_assert(g_hash_table_size(request->priv->args) == 2);
    g_assert(g_hash_table_size(request->priv->cookies) == 2);
    g_assert(request->authorization != NULL);
    g_assert_cmpstr(request->authorization->username, ==, "bola");
    g_assert_cmpstr(request->authorization->password, ==, "guda:lol");
    g_assert(request->priv->view_args == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_make_request_without_path_info_with_script_name(void)
{
    g_setenv("HTTP_LOL_HEHE", "12345", TRUE);
    g_setenv("HTTP_XD_KKK", "asdf", TRUE);
    g_setenv("HTTP_COOKIE", "asd=\"qwe\"; bola=guda", TRUE);
    g_setenv("HTTP_AUTHORIZATION", "Basic Ym9sYTpndWRhOmxvbA==", TRUE);
    g_setenv("REQUEST_METHOD", "GET", TRUE);
    g_setenv("QUERY_STRING", "asd=lol&xd=hehe", TRUE);
    g_setenv("SCRIPT_NAME", "/bola/", TRUE);
    g_unsetenv("PATH_INFO");
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    g_assert_cmpstr(request->server_name, ==, "bola");
    g_assert_cmpstr(request->path, ==, "/bola/");
    g_assert(request->script_name == NULL);
    g_assert(request->method == BALDE_HTTP_GET);
    g_assert(g_hash_table_size(request->priv->headers) == 4);
    g_assert(g_hash_table_size(request->priv->args) == 2);
    g_assert(g_hash_table_size(request->priv->cookies) == 2);
    g_assert(request->authorization != NULL);
    g_assert_cmpstr(request->authorization->username, ==, "bola");
    g_assert_cmpstr(request->authorization->password, ==, "guda:lol");
    g_assert(request->priv->view_args == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_make_request_with_env(void)
{
    balde_request_env_t *env = g_new(balde_request_env_t, 1);
    env->server_name = g_strdup("localhost");
    env->script_name = NULL;
    env->path_info = g_strdup("/");
    env->request_method = g_strdup("GET");
    env->query_string = g_strdup("asd=lol&xd=hehe");
    env->headers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
        g_free);
    g_hash_table_replace(env->headers, g_strdup("lol-hehe"), g_strdup("12345"));
    g_hash_table_replace(env->headers, g_strdup("xd"), g_strdup("asdf"));
    g_hash_table_replace(env->headers, g_strdup("cookie"),
        g_strdup("asd=\"qwe\"; bola=guda"));
    g_hash_table_replace(env->headers, g_strdup("authorization"),
        g_strdup("Basic Ym9sYTpndWRhOmxvbA=="));
    env->body = NULL;
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, env);
    g_assert_cmpstr(request->path, ==, "/");
    g_assert_cmpstr(request->server_name, ==, "localhost");
    g_assert(request->method == BALDE_HTTP_GET);
    g_assert(g_hash_table_size(request->priv->headers) == 4);
    g_assert(g_hash_table_size(request->priv->args) == 2);
    g_assert(g_hash_table_size(request->priv->cookies) == 2);
    g_assert(request->authorization != NULL);
    g_assert_cmpstr(request->authorization->username, ==, "bola");
    g_assert_cmpstr(request->authorization->password, ==, "guda:lol");
    g_assert(request->priv->view_args == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_make_request_with_body(void)
{
    balde_request_env_t *env = g_new(balde_request_env_t, 1);
    env->server_name = g_strdup("localhost");
    env->script_name = NULL;
    env->path_info = g_strdup("/");
    env->request_method = g_strdup("POST");
    env->query_string = g_strdup("asd=lol&xd=hehe");
    env->headers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
        g_free);
    g_hash_table_replace(env->headers, g_strdup("content-type"),
        g_strdup(
            "multipart/form-data; boundary=---------------------------"
            "12056991879709948637505812"));
    g_hash_table_replace(env->headers, g_strdup("xd"), g_strdup("asdf"));
    g_hash_table_replace(env->headers, g_strdup("cookie"),
        g_strdup("asd=\"qwe\"; bola=guda"));
    g_hash_table_replace(env->headers, g_strdup("authorization"),
        g_strdup("Basic Ym9sYTpndWRhOmxvbA=="));
    GString *body = get_upload("simple.txt");
    env->body = body;
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, env);
    g_assert_cmpstr(request->path, ==, "/");
    g_assert_cmpstr(request->server_name, ==, "localhost");
    g_assert(request->method == BALDE_HTTP_POST);
    g_assert(g_hash_table_size(request->priv->headers) == 4);
    g_assert(g_hash_table_size(request->priv->args) == 2);
    g_assert(g_hash_table_size(request->priv->cookies) == 2);
    g_assert(request->authorization != NULL);
    g_assert_cmpstr(request->authorization->username, ==, "bola");
    g_assert_cmpstr(request->authorization->password, ==, "guda:lol");
    g_assert(request->priv->view_args == NULL);
    g_assert_cmpint(g_hash_table_size(request->priv->form), ==, 1);
    g_assert_cmpstr(g_hash_table_lookup(request->priv->form, "name"), ==,
        "chunda");
    g_assert_cmpint(g_hash_table_size(request->priv->files), ==, 1);
    g_assert(g_hash_table_lookup(request->priv->files, "bola") != NULL);
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
    balde_request_t *request = balde_make_request(app, NULL);
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
    balde_request_t *request = balde_make_request(app, NULL);
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
    balde_request_t *request = balde_make_request(app, NULL);
    g_assert(request->priv->body == NULL);
    g_assert(request->priv->form == NULL);
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
    balde_request_t *request = balde_make_request(app, NULL);
    g_assert(request->priv->body == NULL);
    g_assert(g_hash_table_size(request->priv->form) == 0);
    g_assert(balde_request_get_form(request, "lol") == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_request_get_file(void)
{
    g_setenv("REQUEST_METHOD", "GET", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    g_assert(request->priv->body == NULL);
    g_assert(request->priv->files == NULL);
    g_assert(balde_request_get_file(request, "lol") == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_request_get_file_with_empty_body(void)
{
    g_setenv("REQUEST_METHOD", "POST", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    // ommited CONTENT_LENGTH
    balde_request_t *request = balde_make_request(app, NULL);
    g_assert(request->priv->body == NULL);
    g_assert(request->priv->files == NULL);
    g_assert(balde_request_get_file(request, "lol") == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_request_get_view_arg(void)
{
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->view_args = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_replace(request->priv->view_args, g_strdup("foo"), g_strdup("bar"));
    g_assert_cmpstr(balde_request_get_view_arg(request, "foo"), == , "bar");
    g_assert(balde_request_get_view_arg(request, "xd") == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_request_get_cookie(void)
{
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    g_hash_table_replace(request->priv->cookies, g_strdup("foo"), g_strdup("bar"));
    g_assert_cmpstr(balde_request_get_cookie(request, "foo"), == , "bar");
    g_assert(balde_request_get_cookie(request, "xd") == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_request_get_body(void)
{
    g_setenv("REQUEST_METHOD", "GET", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    g_assert(balde_request_get_body(request) == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_request_get_body_with_empty_body(void)
{
    g_setenv("REQUEST_METHOD", "POST", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    // ommited CONTENT_LENGTH
    balde_request_t *request = balde_make_request(app, NULL);
    const GString *str = balde_request_get_body(request);
    g_assert(str == NULL);
    balde_request_free(request);
    balde_app_free(app);
}


typedef struct {
    gchar *tmpdir;
} tmpdir_fixture_t;


void
tmpdir_setup(tmpdir_fixture_t *f, gconstpointer data)
{
    f->tmpdir = g_build_filename(g_get_tmp_dir(), "test.balde.XXXXXX", NULL);
    f->tmpdir = g_mkdtemp(f->tmpdir);
}


void
tmpdir_teardown(tmpdir_fixture_t *f, gconstpointer data)
{
    g_rmdir(f->tmpdir);
    g_free(f->tmpdir);
}


void
tmpdir_runner(tmpdir_fixture_t *f, gconstpointer data)
{
    ((void (*) (const gchar*)) data) (f->tmpdir);
}


void
test_file_save_to_disk(gchar *tmpdir)
{
    balde_file_t *f = g_new(balde_file_t, 1);
    f->name = g_strdup("bola.txt");
    f->type = NULL;
    f->content = g_string_new("bola\nguda\n");
    gchar *fname = balde_file_save_to_disk(f, tmpdir, NULL);
    g_assert(g_str_has_suffix(fname, "bola.txt"));
    gchar *contents;
    gsize len;
    g_file_get_contents(fname, &contents, &len, NULL);
    g_assert_cmpstr(contents, ==, "bola\nguda\n");
    g_assert_cmpint(len, ==, 10);
    g_free(contents);
    g_unlink(fname);
    g_free(fname);
    balde_file_free(f);
}


void
test_file_save_to_disk_with_new_name(gchar *tmpdir)
{
    balde_file_t *f = g_new(balde_file_t, 1);
    f->name = g_strdup("bola.txt");
    f->type = NULL;
    f->content = g_string_new("bola\nguda\n");
    gchar *fname = balde_file_save_to_disk(f, tmpdir, "chunda.txt");
    g_assert(g_str_has_suffix(fname, "chunda.txt"));
    gchar *contents;
    gsize len;
    g_file_get_contents(fname, &contents, &len, NULL);
    g_assert_cmpstr(contents, ==, "bola\nguda\n");
    g_assert_cmpint(len, ==, 10);
    g_free(contents);
    g_unlink(fname);
    g_free(fname);
    balde_file_free(f);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/requests/urldecode", test_urldecode);
    g_test_add_func("/requests/parse_query_string", test_parse_query_string);
    g_test_add_func("/requests/parse_cookies", test_parse_cookies);
    g_test_add_func("/requests/parse_authorization", test_parse_authorization);
    g_test_add_func("/requests/make_request", test_make_request);
    g_test_add_func("/requests/make_request_without_path_info",
        test_make_request_without_path_info);
    g_test_add_func("/requests/make_request_without_path_info_with_script_name",
        test_make_request_without_path_info_with_script_name);
    g_test_add_func("/requests/make_request_with_env", test_make_request_with_env);
    g_test_add_func("/requests/make_request_with_body", test_make_request_with_body);
    g_test_add_func("/requests/get_header", test_request_get_header);
    g_test_add_func("/requests/get_arg", test_request_get_arg);
    g_test_add_func("/requests/get_form", test_request_get_form);
    g_test_add_func("/requests/get_form_with_empty_body",
        test_request_get_form_with_empty_body);
    g_test_add_func("/requests/get_file", test_request_get_file);
    g_test_add_func("/requests/get_file_with_empty_body",
        test_request_get_file_with_empty_body);
    g_test_add_func("/requests/get_view_arg", test_request_get_view_arg);
    g_test_add_func("/requests/get_cookie", test_request_get_cookie);
    g_test_add_func("/requests/get_body", test_request_get_body);
    g_test_add_func("/requests/get_body_with_empty_body",
        test_request_get_body_with_empty_body);
    g_test_add("/requests/file_save_to_disk", tmpdir_fixture_t,
        (gpointer) test_file_save_to_disk, tmpdir_setup, tmpdir_runner,
        tmpdir_teardown);
    g_test_add("/requests/file_save_to_disk_with_new_name", tmpdir_fixture_t,
        (gpointer) test_file_save_to_disk_with_new_name, tmpdir_setup,
        tmpdir_runner, tmpdir_teardown);
    return g_test_run();
}
