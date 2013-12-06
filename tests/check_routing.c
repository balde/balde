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
#include "routing.h"


const balde_url_rule_t rules[] = {
    {"home", "/"},
    {"user", "/user/<username>/"},
    {"customer", "/customer/<username>/contracts/"},
    {NULL, NULL}
};


void
test_url_match(void)
{
    GHashTable *matches = NULL;
    gboolean match = balde_url_match("/lol/", "/lol/", &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 0);
    g_hash_table_destroy(matches);
}


void
test_url_match_with_variable(void)
{
    GHashTable *matches = NULL;
    gboolean match = balde_url_match("/lol/hehe/", "/lol/<asd>/", &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 1);
    g_assert_cmpstr(g_hash_table_lookup(matches, "asd"), ==, "hehe");
    g_hash_table_destroy(matches);
}


void
test_url_match_with_null_path(void)
{
    GHashTable *matches = NULL;
    gboolean match = balde_url_match(NULL, "/", &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 0);
    g_hash_table_destroy(matches);
}


void
test_url_match_with_empty_path(void)
{
    GHashTable *matches = NULL;
    gboolean match = balde_url_match("", "/", &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 0);
    g_hash_table_destroy(matches);
}


void
test_url_match_without_trailing_slash(void)
{
    GHashTable *matches = NULL;
    gboolean match = balde_url_match("/test/asd", "/test/asd", &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 0);
    g_hash_table_destroy(matches);
}


void
test_url_match_without_trailing_slash_with_variable(void)
{
    GHashTable *matches = NULL;
    gboolean match = balde_url_match("/test/asd", "/test/<lol>", &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 1);
    g_assert_cmpstr(g_hash_table_lookup(matches, "lol"), ==, "asd");
    g_hash_table_destroy(matches);
}


void
test_url_match_with_multiple_matches(void)
{
    GHashTable *matches = NULL;
    gboolean match = balde_url_match("/test/foo/tset/bar/test/baz/",
        "/test/<lol>/tset/<hehe>/test/<xd>/", &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 3);
    g_assert_cmpstr(g_hash_table_lookup(matches, "lol"), ==, "foo");
    g_assert_cmpstr(g_hash_table_lookup(matches, "hehe"), ==, "bar");
    g_assert_cmpstr(g_hash_table_lookup(matches, "xd"), ==, "baz");
    g_hash_table_destroy(matches);
}


void
test_url_no_match(void)
{
    GHashTable *matches = NULL;
    gboolean match = balde_url_match("/test/foo/", "/test/fool/", &matches);
    g_assert(!match);
    g_assert(matches == NULL);
}


void
test_url_no_match2(void)
{
    GHashTable *matches = NULL;
    gboolean match = balde_url_match("/test/foo/", "/test/", &matches);
    g_assert(!match);
    g_assert(matches == NULL);
}


void
test_url_rule(void)
{
    GHashTable *matches = NULL;
    gchar* endpoint = balde_dispatch_from_path(rules, "/user/arcoiro/",
        &matches);
    g_assert_cmpstr(endpoint, ==, "user");
    g_assert_cmpstr(g_hash_table_lookup(matches, "username"), ==, "arcoiro");
    g_hash_table_destroy(matches);
}


void
test_url_rule_not_found(void)
{
    GHashTable *matches = NULL;
    gchar* endpoint = balde_dispatch_from_path(rules, "/bola/arcoiro/",
        &matches);
    g_assert(endpoint == NULL);
    g_assert(matches == NULL);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/routing/url_match", test_url_match);
    g_test_add_func("/routing/url_match_with_variable",
        test_url_match_with_variable);
    g_test_add_func("/routing/url_match_with_null_path",
        test_url_match_with_null_path);
    g_test_add_func("/routing/url_match_with_empty_path",
        test_url_match_with_empty_path);
    g_test_add_func("/routing/url_match_without_trailing_slash",
        test_url_match_without_trailing_slash);
    g_test_add_func("/routing/url_match_without_trailing_slash_with_variable",
        test_url_match_without_trailing_slash_with_variable);
    g_test_add_func("/routing/url_match_with_multiple_matches",
        test_url_match_with_multiple_matches);
    g_test_add_func("/routing/url_no_match",
        test_url_no_match);
    g_test_add_func("/routing/url_no_match2",
        test_url_no_match2);
    g_test_add_func("/routing/url_rule", test_url_rule);
    g_test_add_func("/routing/url_rule_not_found",
        test_url_rule_not_found);
    return g_test_run();
}
