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
#include "../src/app.h"
#include "../src/sessions.h"
#include "../src/wrappers.h"

gint64 timestamp = 1357098400;


void
test_session_serialize(void)
{
    GHashTable *h = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(h, (gpointer) g_strdup("bola"), (gpointer) g_strdup("guda"));
    g_hash_table_insert(h, (gpointer) g_strdup("chunda"), (gpointer) g_strdup("asd"));
    gchar *t = balde_session_serialize(h);
    g_assert_cmpstr(t, ==, "eyJib2xhIjoiZ3VkYSIsImNodW5kYSI6ImFzZCJ9");
    g_free(t);
    g_hash_table_destroy(h);
}


void
test_session_unserialize(void)
{
    // {"bola":"guda","chunda":"asd"}
    const gchar* json = "eyJib2xhIjoiZ3VkYSIsImNodW5kYSI6ImFzZCJ9";
    GHashTable *session = balde_session_unserialize(json);
    g_assert(session != NULL);
    g_assert_cmpstr(g_hash_table_lookup(session, "bola"), ==, "guda");
    g_assert_cmpstr(g_hash_table_lookup(session, "chunda"), ==, "asd");
    g_hash_table_destroy(session);
}


void
test_session_unserialize_broken_json(void)
{
    // {"bola":"guda","chunda": }
    const gchar* json = "eyJib2xhIjoiZ3VkYSIsImNodW5kYSI6IH0=";
    GHashTable *session = balde_session_unserialize(json);
    g_assert(session == NULL);
}


void
test_session_unserialize_wrong_type(void)
{
    // {"bola":"guda","chunda": 1234,"asd":"qwe"}
    const gchar* json = "eyJib2xhIjoiZ3VkYSIsImNodW5kYSI6IDEyMzQsImFzZCI6InF3ZSJ9";
    GHashTable *session = balde_session_unserialize(json);
    g_assert(session == NULL);
}


void
test_session_derive_key(void)
{
    gchar *key = balde_session_derive_key((guchar*) "guda", 4);
    g_assert_cmpstr(key, ==, "94a702e385b8c76d636610137ae654a6ad2d1e01");
    g_free(key);
}


void
test_session_sign(void)
{
    gchar *signed_str = balde_session_sign((guchar*) "guda", 4, "bola");
    g_assert_cmpstr(signed_str, ==,
        "bola|MTAwMDAw.24f2e79663f85946381adcaf5e87687cfa4a610c");
    g_free(signed_str);
}


void
test_session_unsign(void)
{
    timestamp = 1357098400;
    gsize len;
    gchar *content;
    balde_session_unsign_status_t status = balde_session_unsign((guchar*) "guda",
        4, 40, "bola|MTAwMDAw.4bf2fd5c755f810d27973750c832b0b818250f13", &content);
    g_assert_cmpint(status, ==, BALDE_SESSION_UNSIGN_OK);
    g_assert_cmpstr(content, ==, "bola");
    g_free(content);
}


void
test_session_unsign_bad_format(void)
{
    timestamp = 1357098400;
    gsize len;
    gchar *content;
    balde_session_unsign_status_t status = balde_session_unsign((guchar*) "guda",
        4, 40, "bola|MTAwMDAw4bf2fd5c755f810d27973750c832b0b818250f13", &content);
    g_assert_cmpint(status, ==, BALDE_SESSION_UNSIGN_BAD_FORMAT);
    g_assert(content == NULL);
    status = balde_session_unsign((guchar*) "guda", 4, 40,
        "bolaMTAwMDAw4bf2fd5c755f810d27973750c832b0b818250f13", &content);
    g_assert_cmpint(status, ==, BALDE_SESSION_UNSIGN_BAD_FORMAT);
    g_assert(content == NULL);
}


void
test_session_unsign_bad_timestamp(void)
{
    timestamp = 1357099400;
    gsize len;
    gchar *content;
    balde_session_unsign_status_t status = balde_session_unsign((guchar*) "guda",
        4, 40, "bola|MTAwMDAw.4bf2fd5c755f810d27973750c832b0b818250f13", &content);
    g_assert_cmpint(status, ==, BALDE_SESSION_UNSIGN_BAD_TIMESTAMP);
    g_assert(content == NULL);
}


void
test_session_unsign_bad_sign(void)
{
    timestamp = 1357098400;
    gchar *content;
    balde_session_unsign_status_t status = balde_session_unsign((guchar*) "guda",
        4, 40, "bola|MTAwMDAw.4bf2fd5c755f810d27973750c832b0b818250f14", &content);
    g_assert_cmpint(status, ==, BALDE_SESSION_UNSIGN_BAD_SIGN);
    g_assert(content == NULL);
}


void
test_session_open(void)
{
    timestamp = 1357098400;
    g_unsetenv("HTTP_COOKIE");
    g_setenv("SERVER_NAME", "guda", TRUE);
    g_setenv("SCRIPT_NAME", "/bola", TRUE);
    g_setenv("PATH_INFO", "/", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_app_set_config(app, "SECRET_KEY", "bola");
    balde_request_t *request = balde_make_request(app, NULL);
    balde_session_t *session = balde_session_open(app, request);

    g_assert(app->error == NULL);
    g_assert(session != NULL);
    g_assert(session->storage == NULL);
    g_assert(!session->secure);
    g_assert_cmpstr(session->key, ==, "bola");
    g_assert_cmpint(session->key_len, ==, 4);
    g_assert_cmpstr(session->path, ==, "/bola");
    g_assert_cmpstr(session->domain, ==, "guda");
    g_assert_cmpint(session->max_age, ==, 2678400);

    g_free(session);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_session_open_no_secret_key(void)
{
    timestamp = 1357098400;
    g_unsetenv("HTTP_COOKIE");
    g_setenv("SERVER_NAME", "guda", TRUE);
    g_setenv("SCRIPT_NAME", "/bola", TRUE);
    g_setenv("PATH_INFO", "/", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    balde_session_t *session = balde_session_open(app, request);

    g_assert(app->error != NULL);
    g_assert_cmpint(app->error->code, ==, 500);
    g_assert(session == NULL);

    balde_request_free(request);
    balde_app_free(app);
}


void
test_session_open_with_cookie(void)
{
    timestamp = 1357098400;
    g_setenv("HTTP_COOKIE", "balde_session=\"eyJjaHVuZGEiOiAibG9saGVoZSJ9|MTAwMDAw."
        "ce018465cdce573f4616db012499f57f230d770a\"", TRUE);
    g_setenv("SERVER_NAME", "guda", TRUE);
    g_setenv("SCRIPT_NAME", "/bola", TRUE);
    g_setenv("PATH_INFO", "/", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_app_set_config(app, "SECRET_KEY", "guda");
    balde_request_t *request = balde_make_request(app, NULL);
    balde_session_t *session = balde_session_open(app, request);

    g_assert(app->error == NULL);
    g_assert(session != NULL);
    g_assert(session->storage != NULL);
    g_assert_cmpint(g_hash_table_size(session->storage), ==, 1);
    g_assert_cmpstr(g_hash_table_lookup(session->storage, "chunda"), ==, "lolhehe");
    g_assert(!session->secure);
    g_assert_cmpstr(session->key, ==, "guda");
    g_assert_cmpint(session->key_len, ==, 4);
    g_assert_cmpstr(session->path, ==, "/bola");
    g_assert_cmpstr(session->domain, ==, "guda");
    g_assert_cmpint(session->max_age, ==, 2678400);

    g_hash_table_destroy(session->storage);
    g_free(session);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_session_open_with_cookie_and_key_length(void)
{
    timestamp = 1357098400;
    g_setenv("HTTP_COOKIE", "balde_session=\"eyJjaHVuZGEiOiAibG9saGVoZSJ9|MTAwMDAw."
        "ce018465cdce573f4616db012499f57f230d770a\"", TRUE);
    g_setenv("SERVER_NAME", "guda", TRUE);
    g_setenv("SCRIPT_NAME", "/bola", TRUE);
    g_setenv("PATH_INFO", "/", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_app_set_config(app, "SECRET_KEY", "guda-moises");
    balde_app_set_config(app, "SECRET_KEY_LENGTH", "4");
    balde_request_t *request = balde_make_request(app, NULL);
    balde_session_t *session = balde_session_open(app, request);

    g_assert(app->error == NULL);
    g_assert(session != NULL);
    g_assert(session->storage != NULL);
    g_assert_cmpint(g_hash_table_size(session->storage), ==, 1);
    g_assert_cmpstr(g_hash_table_lookup(session->storage, "chunda"), ==, "lolhehe");
    g_assert(!session->secure);
    g_assert_cmpstr(session->key, ==, "guda-moises");
    g_assert_cmpint(session->key_len, ==, 4);
    g_assert_cmpstr(session->path, ==, "/bola");
    g_assert_cmpstr(session->domain, ==, "guda");
    g_assert_cmpint(session->max_age, ==, 2678400);

    g_hash_table_destroy(session->storage);
    g_free(session);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_session_save(void)
{
    timestamp = 1357098400;
    balde_response_t *response = balde_make_response("");
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(session->storage, g_strdup("bola"), g_strdup("guda"));
    g_hash_table_insert(session->storage, g_strdup("asd"), g_strdup("lolhehe"));
    session->secure = TRUE;
    session->key = "bola";
    session->key_len = 4;
    session->path = "/";
    session->domain = "guda";
    session->max_age = 2678400;

    balde_session_save(response, session);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"eyJhc2QiOiJsb2xoZWhlIiwiYm9sYSI6Imd1ZGEifQ==|MTAwMDAw"
        ".7e9e8a554349a652636b1d53cda974eccf5b8fd2\"; Domain=\".guda\"; Expires"
        "=Sat, 02-Feb-2013 03:46:40 GMT; Max-Age=2678400; Secure; HttpOnly; "
        "Path=/");

    balde_response_free(response);
}


void
test_session_save_server_name_with_port(void)
{
    timestamp = 1357098400;
    balde_response_t *response = balde_make_response("");
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(session->storage, g_strdup("bola"), g_strdup("guda"));
    g_hash_table_insert(session->storage, g_strdup("asd"), g_strdup("lolhehe"));
    session->secure = TRUE;
    session->key = "bola";
    session->key_len = 4;
    session->path = "/";
    session->domain = "chunda:8080";
    session->max_age = 2678400;

    balde_session_save(response, session);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"eyJhc2QiOiJsb2xoZWhlIiwiYm9sYSI6Imd1ZGEifQ==|MTAwMDAw"
        ".7e9e8a554349a652636b1d53cda974eccf5b8fd2\"; Domain=\".chunda\"; Expires"
        "=Sat, 02-Feb-2013 03:46:40 GMT; Max-Age=2678400; Secure; HttpOnly; "
        "Path=/");

    balde_response_free(response);
}


void
test_session_save_with_path(void)
{
    timestamp = 1357098400;
    balde_response_t *response = balde_make_response("");
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(session->storage, g_strdup("bola"), g_strdup("guda"));
    g_hash_table_insert(session->storage, g_strdup("asd"), g_strdup("lolhehe"));
    session->secure = TRUE;
    session->key = "bola";
    session->key_len = 4;
    session->path = "/bola";
    session->domain = "guda";
    session->max_age = 2678400;

    balde_session_save(response, session);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"eyJhc2QiOiJsb2xoZWhlIiwiYm9sYSI6Imd1ZGEifQ==|MTAwMDAw"
        ".7e9e8a554349a652636b1d53cda974eccf5b8fd2\"; Domain=\"guda\"; Expires"
        "=Sat, 02-Feb-2013 03:46:40 GMT; Max-Age=2678400; Secure; HttpOnly; "
        "Path=/bola");

    balde_response_free(response);
}


void
test_session_save_with_null_path(void)
{
    timestamp = 1357098400;
    balde_response_t *response = balde_make_response("");
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(session->storage, g_strdup("bola"), g_strdup("guda"));
    g_hash_table_insert(session->storage, g_strdup("asd"), g_strdup("lolhehe"));
    session->secure = TRUE;
    session->key = "bola";
    session->key_len = 4;
    session->path = NULL;
    session->domain = "guda";
    session->max_age = 2678400;

    balde_session_save(response, session);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"eyJhc2QiOiJsb2xoZWhlIiwiYm9sYSI6Imd1ZGEifQ==|MTAwMDAw"
        ".7e9e8a554349a652636b1d53cda974eccf5b8fd2\"; Domain=\".guda\"; Expires"
        "=Sat, 02-Feb-2013 03:46:40 GMT; Max-Age=2678400; Secure; HttpOnly; "
        "Path=/");

    balde_response_free(response);
}


void
test_session_save_with_null_domain(void)
{
    timestamp = 1357098400;
    balde_response_t *response = balde_make_response("");
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(session->storage, g_strdup("bola"), g_strdup("guda"));
    g_hash_table_insert(session->storage, g_strdup("asd"), g_strdup("lolhehe"));
    session->secure = TRUE;
    session->key = "bola";
    session->key_len = 4;
    session->path = NULL;
    session->domain = NULL;
    session->max_age = 2678400;

    balde_session_save(response, session);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"eyJhc2QiOiJsb2xoZWhlIiwiYm9sYSI6Imd1ZGEifQ==|MTAwMDAw"
        ".7e9e8a554349a652636b1d53cda974eccf5b8fd2\"; Expires"
        "=Sat, 02-Feb-2013 03:46:40 GMT; Max-Age=2678400; Secure; HttpOnly; "
        "Path=/");

    balde_response_free(response);
}


void
test_session_save_with_localhost(void)
{
    timestamp = 1357098400;
    balde_response_t *response = balde_make_response("");
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(session->storage, g_strdup("bola"), g_strdup("guda"));
    g_hash_table_insert(session->storage, g_strdup("asd"), g_strdup("lolhehe"));
    session->secure = TRUE;
    session->key = "bola";
    session->key_len = 4;
    session->path = NULL;
    session->domain = "localhost";
    session->max_age = 2678400;

    balde_session_save(response, session);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"eyJhc2QiOiJsb2xoZWhlIiwiYm9sYSI6Imd1ZGEifQ==|MTAwMDAw"
        ".7e9e8a554349a652636b1d53cda974eccf5b8fd2\"; Expires"
        "=Sat, 02-Feb-2013 03:46:40 GMT; Max-Age=2678400; Secure; HttpOnly; "
        "Path=/");

    balde_response_free(response);
}


void
test_session_save_empty(void)
{
    timestamp = 1357098400;
    balde_response_t *response = balde_make_response("");
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = NULL;
    session->secure = TRUE;
    session->key = "bola";
    session->key_len = 4;
    session->path = "/bola";
    session->domain = "guda";
    session->max_age = 2678400;

    balde_session_save(response, session);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 0);

    balde_response_free(response);
}


void
test_session_get(void)
{
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(session->storage, g_strdup("bola"), g_strdup("guda"));
    g_assert_cmpstr(balde_session_get(session, "bola"), ==, "guda");
    g_hash_table_destroy(session->storage);
    g_free(session);
}


void
test_session_get_not_found(void)
{
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_assert(balde_session_get(session, "bola") == NULL);
    g_hash_table_destroy(session->storage);
    g_free(session);
}


void
test_session_set(void)
{
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = NULL;
    balde_session_set(session, "bola", "guda");
    g_assert_cmpint(g_hash_table_size(session->storage), ==, 1);
    g_assert_cmpstr(g_hash_table_lookup(session->storage, "bola"), ==, "guda");
    balde_session_set(session, "chunda", "lolhehe");
    g_assert_cmpint(g_hash_table_size(session->storage), ==, 2);
    g_assert_cmpstr(g_hash_table_lookup(session->storage, "bola"), ==, "guda");
    g_assert_cmpstr(g_hash_table_lookup(session->storage, "chunda"), ==, "lolhehe");
    g_hash_table_destroy(session->storage);
    g_free(session);
}


void
test_session_delete(void)
{
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(session->storage, g_strdup("bola"), g_strdup("guda"));
    balde_session_delete(session, "bola");
    g_assert_cmpint(g_hash_table_size(session->storage), ==, 0);
    g_hash_table_destroy(session->storage);
    g_free(session);
}


void
test_session_delete_not_found(void)
{
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(session->storage, g_strdup("bola"), g_strdup("guda"));
    balde_session_delete(session, "chunda");
    g_assert_cmpint(g_hash_table_size(session->storage), ==, 1);
    g_hash_table_destroy(session->storage);
    g_free(session);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/sessions/serialize", test_session_serialize);
    g_test_add_func("/sessions/unserialize", test_session_unserialize);
    g_test_add_func("/sessions/unserialize_broken_json",
        test_session_unserialize_broken_json);
    g_test_add_func("/sessions/unserialize_wrong_type",
        test_session_unserialize_wrong_type);
    g_test_add_func("/sessions/derive_key", test_session_derive_key);
    g_test_add_func("/sessions/sign", test_session_sign);
    g_test_add_func("/sessions/unsign", test_session_unsign);
    g_test_add_func("/sessions/unsign_bad_format",
        test_session_unsign_bad_format);
    g_test_add_func("/sessions/unsign_bad_timestamp",
        test_session_unsign_bad_timestamp);
    g_test_add_func("/sessions/unsign_bad_sign", test_session_unsign_bad_sign);
    g_test_add_func("/sessions/open", test_session_open);
    g_test_add_func("/sessions/open_no_secret_key",
        test_session_open_no_secret_key);
    g_test_add_func("/sessions/open_with_cookie", test_session_open_with_cookie);
    g_test_add_func("/sessions/open_with_cookie_and_key_length",
        test_session_open_with_cookie_and_key_length);
    g_test_add_func("/sessions/save", test_session_save);
    g_test_add_func("/sessions/save_server_name_with_port",
        test_session_save_server_name_with_port);
    g_test_add_func("/sessions/save_with_path", test_session_save_with_path);
    g_test_add_func("/sessions/save_with_null_path",
        test_session_save_with_null_path);
    g_test_add_func("/sessions/save_with_null_domain",
        test_session_save_with_null_domain);
    g_test_add_func("/sessions/save_with_localhost",
        test_session_save_with_localhost);
    g_test_add_func("/sessions/save_empty", test_session_save_empty);
    g_test_add_func("/sessions/get", test_session_get);
    g_test_add_func("/sessions/get_not_found", test_session_get_not_found);
    g_test_add_func("/sessions/set", test_session_set);
    g_test_add_func("/sessions/delete", test_session_delete);
    g_test_add_func("/sessions/delete_not_found", test_session_delete_not_found);
    return g_test_run();
}
