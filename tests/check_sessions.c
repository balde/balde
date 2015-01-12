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
#include "../src/sessions.h"

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
    g_test_add_func("/sessions/sign", test_session_sign);
    g_test_add_func("/sessions/unsign", test_session_unsign);
    g_test_add_func("/sessions/unsign_bad_format",
        test_session_unsign_bad_format);
    g_test_add_func("/sessions/unsign_bad_timestamp",
        test_session_unsign_bad_timestamp);
    g_test_add_func("/sessions/unsign_bad_sign", test_session_unsign_bad_sign);
    return g_test_run();
}
