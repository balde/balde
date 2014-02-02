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
#include <balde/routing.h>
#include <balde/routing-private.h>

static balde_url_rule_t rules[] = {
    {"home", "/", NULL, BALDE_HTTP_GET},
    {"user", "/user/<username>/", NULL, BALDE_HTTP_POST},
    {"customer", "/customer/<username>/contracts/", NULL, BALDE_HTTP_GET},
    {"policy", "/policies/", NULL, BALDE_HTTP_GET | BALDE_HTTP_POST},
    {NULL, NULL, NULL, 0}
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
    gchar* endpoint = balde_dispatch_from_path(views, "/user/arcoiro/", &matches);
    g_assert_cmpstr(endpoint, ==, "user");
    g_assert_cmpstr(g_hash_table_lookup(matches, "username"), ==, "arcoiro");
    g_free(endpoint);
    g_hash_table_destroy(matches);
    free_test_views(views);
}


void
test_url_rule_not_found(void)
{
    GSList *views = get_test_views();
    GHashTable *matches = NULL;
    gchar* endpoint = balde_dispatch_from_path(views, "/bola/arcoiro/", &matches);
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
    endpoint = balde_dispatch_from_path(views, "/policies/", &matches);
    g_assert_cmpstr(endpoint, ==, "policy");
    g_free(endpoint);
    g_hash_table_destroy(matches);
    free_test_views(views);
}


void
test_http_method_str2enum(void)
{
    balde_http_method_t method;

    method = balde_http_method_str2enum("OPTIONS");
    g_assert(method & BALDE_HTTP_OPTIONS);
    g_assert(!(method & BALDE_HTTP_GET));
    g_assert(!(method & BALDE_HTTP_HEAD));
    g_assert(!(method & BALDE_HTTP_POST));
    g_assert(!(method & BALDE_HTTP_PUT));
    g_assert(!(method & BALDE_HTTP_PATCH));
    g_assert(!(method & BALDE_HTTP_DELETE));

    method = balde_http_method_str2enum("GET");
    g_assert(!(method & BALDE_HTTP_OPTIONS));
    g_assert(method & BALDE_HTTP_GET);
    g_assert(!(method & BALDE_HTTP_HEAD));
    g_assert(!(method & BALDE_HTTP_POST));
    g_assert(!(method & BALDE_HTTP_PUT));
    g_assert(!(method & BALDE_HTTP_PATCH));
    g_assert(!(method & BALDE_HTTP_DELETE));

    method = balde_http_method_str2enum("HEAD");
    g_assert(!(method & BALDE_HTTP_OPTIONS));
    g_assert(!(method & BALDE_HTTP_GET));
    g_assert(method & BALDE_HTTP_HEAD);
    g_assert(!(method & BALDE_HTTP_POST));
    g_assert(!(method & BALDE_HTTP_PUT));
    g_assert(!(method & BALDE_HTTP_PATCH));
    g_assert(!(method & BALDE_HTTP_DELETE));

    method = balde_http_method_str2enum("POST");
    g_assert(!(method & BALDE_HTTP_OPTIONS));
    g_assert(!(method & BALDE_HTTP_GET));
    g_assert(!(method & BALDE_HTTP_HEAD));
    g_assert(method & BALDE_HTTP_POST);
    g_assert(!(method & BALDE_HTTP_PUT));
    g_assert(!(method & BALDE_HTTP_PATCH));
    g_assert(!(method & BALDE_HTTP_DELETE));

    method = balde_http_method_str2enum("PUT");
    g_assert(!(method & BALDE_HTTP_OPTIONS));
    g_assert(!(method & BALDE_HTTP_GET));
    g_assert(!(method & BALDE_HTTP_HEAD));
    g_assert(!(method & BALDE_HTTP_POST));
    g_assert(method & BALDE_HTTP_PUT);
    g_assert(!(method & BALDE_HTTP_PATCH));
    g_assert(!(method & BALDE_HTTP_DELETE));

    method = balde_http_method_str2enum("PATCH");
    g_assert(!(method & BALDE_HTTP_OPTIONS));
    g_assert(!(method & BALDE_HTTP_GET));
    g_assert(!(method & BALDE_HTTP_HEAD));
    g_assert(!(method & BALDE_HTTP_POST));
    g_assert(!(method & BALDE_HTTP_PUT));
    g_assert(method & BALDE_HTTP_PATCH);
    g_assert(!(method & BALDE_HTTP_DELETE));

    method = balde_http_method_str2enum("DELETE");
    g_assert(!(method & BALDE_HTTP_OPTIONS));
    g_assert(!(method & BALDE_HTTP_GET));
    g_assert(!(method & BALDE_HTTP_HEAD));
    g_assert(!(method & BALDE_HTTP_POST));
    g_assert(!(method & BALDE_HTTP_PUT));
    g_assert(!(method & BALDE_HTTP_PATCH));
    g_assert(method & BALDE_HTTP_DELETE);
}


void
test_list_allowed_methods(void)
{
    gchar *allow;
    allow = balde_list_allowed_methods(rules[0].method);
    g_assert_cmpstr(allow, ==, "GET");
    g_free(allow);
    allow = balde_list_allowed_methods(rules[1].method);
    g_assert_cmpstr(allow, ==, "POST");
    g_free(allow);
    allow = balde_list_allowed_methods(rules[2].method);
    g_assert_cmpstr(allow, ==, "GET");
    g_free(allow);
    allow = balde_list_allowed_methods(rules[3].method);
    g_assert_cmpstr(allow, ==, "GET, POST");
    g_free(allow);
}


void
test_parse_url_rule(void)
{
    GRegex *reg;
    GError *error = NULL;
    reg = balde_parse_url_rule("/foo/", &error);
    g_assert_cmpstr(g_regex_get_pattern(reg), ==, "/foo/");
    g_regex_unref(reg);
    reg = balde_parse_url_rule("/foo/<bar>/", &error);
    g_assert_cmpstr(g_regex_get_pattern(reg), ==, "/foo/(?P<bar>[^/]+)/");
    g_regex_unref(reg);
    reg = balde_parse_url_rule("/foo/<bar>/<path:lol>/", &error);
    g_assert_cmpstr(g_regex_get_pattern(reg), ==,
        "/foo/(?P<bar>[^/]+)/(?P<lol>[^/].*?)/");
    g_regex_unref(reg);
    reg = balde_parse_url_rule("/foo/<bar>/<path:lol>/<path:baz>/<kkk>/",
        &error);
    g_assert_cmpstr(g_regex_get_pattern(reg), ==,
        "/foo/(?P<bar>[^/]+)/(?P<lol>[^/].*?)/(?P<baz>[^/].*?)/(?P<kkk>[^/]+)/");
    g_regex_unref(reg);
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
    g_test_add_func("/routing/http_method_str2enum", test_http_method_str2enum);
    g_test_add_func("/routing/list_allowed_methods", test_list_allowed_methods);
    g_test_add_func("/routing/parse_url_rule", test_parse_url_rule);
    return g_test_run();
}
