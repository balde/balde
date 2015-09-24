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
#include "../src/requests.h"
#include "../src/responses.h"

gint64 timestamp = 1357098400;


void
test_session_serialize(void)
{
    GHashTable *h = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(h, (gpointer) g_strdup("bola"), (gpointer) g_strdup("guda"));
    g_hash_table_insert(h, (gpointer) g_strdup("chunda"), (gpointer) g_strdup("asd"));
    gchar *t = balde_session_serialize(h);
    g_assert_cmpstr(t, ==, "Ym9sYQBndWRhAGNodW5kYQBhc2QA");
    g_free(t);
    g_hash_table_destroy(h);
}


void
test_session_unserialize(void)
{
    // bola\0guda\0chunda\0asd\0
    const gchar* str = "Ym9sYQBndWRhAGNodW5kYQBhc2QA";
    GHashTable *session = balde_session_unserialize(str);
    g_assert(session != NULL);
    g_assert_cmpstr(g_hash_table_lookup(session, "bola"), ==, "guda");
    g_assert_cmpstr(g_hash_table_lookup(session, "chunda"), ==, "asd");
    g_hash_table_destroy(session);
}


void
test_session_unserialize_broken(void)
{
    // bola\0guda\0chunda\0
    const gchar* str = "Ym9sYQBndWRhAGNodW5kYQA=";
    GHashTable *session = balde_session_unserialize(str);
    g_assert(session != NULL);
    g_assert_cmpint(g_hash_table_size(session), ==, 1);
    g_assert_cmpstr(g_hash_table_lookup(session, "bola"), ==, "guda");
    g_hash_table_destroy(session);
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
        "bola|MTAwMDAw.4bf2fd5c755f810d27973750c832b0b818250f13");
    g_free(signed_str);
}


void
test_session_unsign(void)
{
    timestamp = 1357098400;
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
    g_setenv("HTTPS", "on", TRUE);
    g_setenv("SERVER_NAME", "guda", TRUE);
    g_setenv("SCRIPT_NAME", "/bola", TRUE);
    g_setenv("PATH_INFO", "/", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_app_set_config(app, "SECRET_KEY", "bola");
    balde_request_t *request = balde_make_request(app, NULL);
    balde_session_open(app, request);

    g_assert(app->error == NULL);
    g_assert(request->priv->session != NULL);
    g_assert(request->priv->session->storage == NULL);
    g_assert(request->https);
    g_assert_cmpstr(request->priv->session->key, ==,
        "d1ddfb31487e2d921cd823f42f7336dbd1e181ae");
    g_assert_cmpint(request->priv->session->max_age, ==, 2678400);
    g_assert_cmpstr(request->script_name, ==, "/bola");
    g_assert_cmpstr(request->server_name, ==, "guda");

    g_free(request->priv->session->key);
    g_free(request->priv->session);
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
    balde_session_open(app, request);

    g_assert(app->error != NULL);
    g_assert_cmpint(app->error->code, ==, 500);
    g_assert(request->priv->session == NULL);

    balde_request_free(request);
    balde_app_free(app);
}


void
test_session_open_with_cookie(void)
{
    timestamp = 1357098400;
    g_unsetenv("HTTPS");
    g_setenv("HTTP_COOKIE", "balde_session=\"Y2h1bmRhAGxvbGhlaGUA|MTAwMDAw."
        "467f9e9b57e9a6e03c59edf7aa2e83231c87102d\"", TRUE);
    g_setenv("SERVER_NAME", "guda", TRUE);
    g_setenv("SCRIPT_NAME", "/bola", TRUE);
    g_setenv("PATH_INFO", "/", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_app_set_config(app, "SECRET_KEY", "guda");
    balde_request_t *request = balde_make_request(app, NULL);
    balde_session_open(app, request);

    g_assert(app->error == NULL);
    g_assert(request->priv->session != NULL);
    g_assert(request->priv->session->storage != NULL);
    g_assert_cmpint(g_hash_table_size(request->priv->session->storage), ==, 1);
    g_assert_cmpstr(g_hash_table_lookup(request->priv->session->storage, "chunda"),
        ==, "lolhehe");
    g_assert(!request->https);
    g_assert_cmpstr(request->priv->session->key, ==,
        "94a702e385b8c76d636610137ae654a6ad2d1e01");
    g_assert_cmpint(request->priv->session->max_age, ==, 2678400);
    g_assert_cmpstr(request->script_name, ==, "/bola");
    g_assert_cmpstr(request->server_name, ==, "guda");

    g_hash_table_destroy(request->priv->session->storage);
    g_free(request->priv->session->key);
    g_free(request->priv->session);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_session_open_with_cookie_and_key_length(void)
{
    timestamp = 1357098400;
    g_unsetenv("HTTPS");
    g_setenv("HTTP_COOKIE", "balde_session=\"Y2h1bmRhAGxvbGhlaGUA|MTAwMDAw."
        "467f9e9b57e9a6e03c59edf7aa2e83231c87102d\"", TRUE);
    g_setenv("SERVER_NAME", "guda", TRUE);
    g_setenv("SCRIPT_NAME", "/bola", TRUE);
    g_setenv("PATH_INFO", "/", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_app_set_config(app, "SECRET_KEY", "guda-moises");
    balde_app_set_config(app, "SECRET_KEY_LENGTH", "4");
    balde_request_t *request = balde_make_request(app, NULL);
    balde_session_open(app, request);

    g_assert(app->error == NULL);
    g_assert(request->priv->session != NULL);
    g_assert(request->priv->session->storage != NULL);
    g_assert_cmpint(g_hash_table_size(request->priv->session->storage), ==, 1);
    g_assert_cmpstr(g_hash_table_lookup(request->priv->session->storage, "chunda"),
        ==, "lolhehe");
    g_assert(!request->https);
    g_assert_cmpstr(request->priv->session->key, ==,
        "94a702e385b8c76d636610137ae654a6ad2d1e01");
    g_assert_cmpint(request->priv->session->max_age, ==, 2678400);
    g_assert_cmpstr(request->script_name, ==, "/bola");
    g_assert_cmpstr(request->server_name, ==, "guda");

    g_hash_table_destroy(request->priv->session->storage);
    g_free(request->priv->session->key);
    g_free(request->priv->session);
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
    g_setenv("HTTPS", "on", TRUE);
    g_setenv("SCRIPT_NAME", "/", TRUE);
    g_setenv("SERVER_NAME", "guda", TRUE);
    session->key = g_strdup("bola");
    session->max_age = 2678400;
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->session = session;

    balde_session_save(request, response);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"YXNkAGxvbGhlaGUAYm9sYQBndWRhAA==|MTAwMDAw."
        "6fc0266ed875f5532293e3931c13f1f02cb775a8\"; Domain=\".guda\"; Expires"
        "=Sat, 02-Feb-2013 03:46:40 GMT; Max-Age=2678400; Secure; HttpOnly; "
        "Path=/");

    balde_request_free(request);
    balde_response_free(response);
    balde_app_free(app);
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
    g_setenv("HTTPS", "on", TRUE);
    g_setenv("SCRIPT_NAME", "/", TRUE);
    g_setenv("SERVER_NAME", "chunda:8080", TRUE);
    session->key = g_strdup("bola");
    session->max_age = 2678400;
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->session = session;

    balde_session_save(request, response);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"YXNkAGxvbGhlaGUAYm9sYQBndWRhAA==|MTAwMDAw."
        "6fc0266ed875f5532293e3931c13f1f02cb775a8\"; Domain=\".chunda\"; Expires"
        "=Sat, 02-Feb-2013 03:46:40 GMT; Max-Age=2678400; Secure; HttpOnly; "
        "Path=/");

    balde_request_free(request);
    balde_response_free(response);
    balde_app_free(app);
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
    g_setenv("HTTPS", "on", TRUE);
    g_setenv("SCRIPT_NAME", "/bola", TRUE);
    g_setenv("SERVER_NAME", "guda", TRUE);
    session->key = g_strdup("bola");
    session->max_age = 2678400;
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->session = session;

    balde_session_save(request, response);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"YXNkAGxvbGhlaGUAYm9sYQBndWRhAA==|MTAwMDAw."
        "6fc0266ed875f5532293e3931c13f1f02cb775a8\"; Domain=\"guda\"; Expires"
        "=Sat, 02-Feb-2013 03:46:40 GMT; Max-Age=2678400; Secure; HttpOnly; "
        "Path=/bola");

    balde_request_free(request);
    balde_response_free(response);
    balde_app_free(app);
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
    g_setenv("HTTPS", "on", TRUE);
    g_unsetenv("SCRIPT_NAME");
    g_setenv("SERVER_NAME", "guda", TRUE);
    session->key = g_strdup("bola");
    session->max_age = 2678400;
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->session = session;

    balde_session_save(request, response);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"YXNkAGxvbGhlaGUAYm9sYQBndWRhAA==|MTAwMDAw."
        "6fc0266ed875f5532293e3931c13f1f02cb775a8\"; Domain=\".guda\"; Expires"
        "=Sat, 02-Feb-2013 03:46:40 GMT; Max-Age=2678400; Secure; HttpOnly; "
        "Path=/");

    balde_request_free(request);
    balde_response_free(response);
    balde_app_free(app);
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
    g_setenv("HTTPS", "on", TRUE);
    g_unsetenv("SCRIPT_NAME");
    g_unsetenv("SERVER_NAME");
    session->key = g_strdup("bola");
    session->max_age = 2678400;
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->session = session;

    balde_session_save(request, response);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"YXNkAGxvbGhlaGUAYm9sYQBndWRhAA==|MTAwMDAw."
        "6fc0266ed875f5532293e3931c13f1f02cb775a8\"; Expires"
        "=Sat, 02-Feb-2013 03:46:40 GMT; Max-Age=2678400; Secure; HttpOnly; "
        "Path=/");

    balde_request_free(request);
    balde_response_free(response);
    balde_app_free(app);
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
    g_setenv("HTTPS", "on", TRUE);
    g_unsetenv("SCRIPT_NAME");
    g_setenv("SERVER_NAME", "localhost", TRUE);
    session->key = g_strdup("bola");
    session->max_age = 2678400;
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->session = session;

    balde_session_save(request, response);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"YXNkAGxvbGhlaGUAYm9sYQBndWRhAA==|MTAwMDAw."
        "6fc0266ed875f5532293e3931c13f1f02cb775a8\"; Expires"
        "=Sat, 02-Feb-2013 03:46:40 GMT; Max-Age=2678400; Secure; HttpOnly; "
        "Path=/");

    balde_request_free(request);
    balde_response_free(response);
    balde_app_free(app);
}


void
test_session_save_empty(void)
{
    timestamp = 1357098400;
    balde_response_t *response = balde_make_response("");
    balde_session_t *session = g_new(balde_session_t, 1);
    g_setenv("HTTPS", "on", TRUE);
    g_setenv("SCRIPT_NAME", "/bola", TRUE);
    g_setenv("SERVER_NAME", "guda", TRUE);
    session->storage = NULL;
    session->key = g_strdup("bola");
    session->max_age = 2678400;
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->session = session;

    balde_session_save(request, response);

    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 1);
    GSList *cookie_list = g_hash_table_lookup(response->priv->headers, "set-cookie");
    g_assert(cookie_list != NULL);
    gchar *cookie = cookie_list->data;
    g_assert_cmpstr(cookie, ==,
        "balde_session=\"\"; Domain=\"guda\"; Expires=Thu, 01-Jan-1970 00:00:00 "
        "GMT; Max-Age=0; Path=/bola");

    balde_request_free(request);
    balde_response_free(response);
    balde_app_free(app);
}


void
test_session_get(void)
{
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(session->storage, g_strdup("bola"), g_strdup("guda"));
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->session = session;
    g_assert_cmpstr(balde_session_get(request, "bola"), ==, "guda");
    g_hash_table_destroy(request->priv->session->storage);
    g_free(request->priv->session);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_session_get_not_found(void)
{
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->session = session;
    g_assert(balde_session_get(request, "bola") == NULL);
    g_hash_table_destroy(request->priv->session->storage);
    g_free(request->priv->session);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_session_set(void)
{
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = NULL;
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->session = session;
    balde_session_set(request, "bola", "guda");
    g_assert_cmpint(g_hash_table_size(request->priv->session->storage), ==, 1);
    g_assert_cmpstr(g_hash_table_lookup(request->priv->session->storage, "bola"),
        ==, "guda");
    balde_session_set(request, "chunda", "lolhehe");
    g_assert_cmpint(g_hash_table_size(request->priv->session->storage), ==, 2);
    g_assert_cmpstr(g_hash_table_lookup(request->priv->session->storage, "bola"),
        ==, "guda");
    g_assert_cmpstr(g_hash_table_lookup(request->priv->session->storage, "chunda"),
        ==, "lolhehe");
    g_hash_table_destroy(request->priv->session->storage);
    g_free(request->priv->session);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_session_delete(void)
{
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(session->storage, g_strdup("bola"), g_strdup("guda"));
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->session = session;
    balde_session_delete(request, "bola");
    g_assert_cmpint(g_hash_table_size(request->priv->session->storage), ==, 0);
    g_hash_table_destroy(request->priv->session->storage);
    g_free(request->priv->session);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_session_delete_not_found(void)
{
    balde_session_t *session = g_new(balde_session_t, 1);
    session->storage = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(session->storage, g_strdup("bola"), g_strdup("guda"));
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    request->priv->session = session;
    balde_session_delete(request, "chunda");
    g_assert_cmpint(g_hash_table_size(request->priv->session->storage), ==, 1);
    g_hash_table_destroy(request->priv->session->storage);
    g_free(request->priv->session);
    balde_request_free(request);
    balde_app_free(app);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/sessions/serialize", test_session_serialize);
    g_test_add_func("/sessions/unserialize", test_session_unserialize);
    g_test_add_func("/sessions/unserialize_broken",
        test_session_unserialize_broken);
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
