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
#include <balde/routing.h>


static balde_url_rule_t rules[] = {
    {"home", "/", BALDE_HTTP_GET},
    {"user", "/user/<username>/", BALDE_HTTP_POST},
    {"customer", "/customer/<username>/contracts/", BALDE_HTTP_GET},
    {"policy", "/policies/", BALDE_HTTP_GET | BALDE_HTTP_POST},
    {NULL, NULL, 0}
};


GSList*
get_test_views(void)
{
    GSList *views = NULL;
    balde_view_t *view;
    for (guint i=0; rules[i].endpoint != NULL; i++) {
        view = g_new(balde_view_t, 1);
        view->url_rule = &rules[i];
        view->view_func = NULL;
        views = g_slist_append(views, view);
    }
    return views;
}


void
free_test_views(GSList* views)
{
    for (GSList *tmp = views; tmp != NULL; tmp = g_slist_next(tmp))
        g_free(tmp->data);
    g_slist_free(views);
}


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
    GSList *views = get_test_views();
    GHashTable *matches = NULL;
    gchar* endpoint = balde_dispatch_from_path(views, "/user/arcoiro/",
        BALDE_HTTP_POST, &matches);
    g_assert_cmpstr(endpoint, ==, "user");
    g_assert_cmpstr(g_hash_table_lookup(matches, "username"), ==, "arcoiro");
    g_free(endpoint);
    g_hash_table_destroy(matches);
    g_assert(balde_dispatch_from_path(views, "/user/arcoiro/", BALDE_HTTP_GET,
        &matches) == NULL);
    g_hash_table_destroy(matches);
    free_test_views(views);
}


void
test_url_rule_not_found(void)
{
    GSList *views = get_test_views();
    GHashTable *matches = NULL;
    gchar* endpoint = balde_dispatch_from_path(views, "/bola/arcoiro/",
        BALDE_HTTP_GET, &matches);
    g_assert(endpoint == NULL);
    g_assert(matches == NULL);
    free_test_views(views);
}


void
test_url_rule_with_multiple_methods(void)
{
    GSList *views = get_test_views();
    GHashTable *matches = NULL;
    gchar* endpoint;
    endpoint = balde_dispatch_from_path(views, "/policies/", BALDE_HTTP_POST,
        &matches);
    g_assert_cmpstr(endpoint, ==, "policy");
    g_free(endpoint);
    g_hash_table_destroy(matches);
    endpoint = balde_dispatch_from_path(views, "/policies/", BALDE_HTTP_GET,
        &matches);
    g_assert_cmpstr(endpoint, ==, "policy");
    g_free(endpoint);
    g_hash_table_destroy(matches);
    free_test_views(views);
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
    g_test_add_func("/routing/url_rule_with_multiple_methods",
        test_url_rule_with_multiple_methods);
    return g_test_run();
}
