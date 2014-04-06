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
    {"path", "/foo/<path:p>/bar/", NULL, BALDE_HTTP_GET},
    {NULL, NULL, NULL, 0}
};


GSList*
get_test_views(void)
{
    GSList *views = NULL;
    balde_view_t *view;
    for (guint i=0; rules[i].endpoint != NULL; i++) {
        view = g_new(balde_view_t, 1);
        view->url_rule = g_new(balde_url_rule_t, 1);
        view->url_rule->endpoint = g_strdup(rules[i].endpoint);
        view->url_rule->rule = g_strdup(rules[i].rule);
        view->url_rule->method = rules[i].method;
        view->url_rule->match = balde_parse_url_rule(view->url_rule->rule, NULL);
        view->view_func = NULL;
        views = g_slist_append(views, view);
    }
    return views;
}


void
free_test_views(GSList* views)
{
    for (GSList *tmp = views; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_view_t *view = (balde_view_t*) tmp->data;
        if (view->url_rule->endpoint != NULL)
            g_free((gchar*) view->url_rule->endpoint);
        if (view->url_rule->rule != NULL)
            g_free((gchar*) view->url_rule->rule);
        balde_free_url_rule_match(view->url_rule->match);
        if (view->url_rule != NULL)
            g_free(view->url_rule);
        g_free(view);
    }
    g_slist_free(views);
}


void
test_url_match(void)
{
    GHashTable *matches = NULL;
    balde_url_rule_match_t *m = balde_parse_url_rule("/lol/", NULL);
    gboolean match = balde_url_match("/lol/", m, &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 0);
    balde_free_url_rule_match(m);
    g_hash_table_destroy(matches);
}


void
test_url_match_with_variable(void)
{
    GHashTable *matches = NULL;
    balde_url_rule_match_t *m = balde_parse_url_rule("/lol/<asd>/", NULL);
    gboolean match = balde_url_match("/lol/hehe/", m, &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 1);
    g_assert_cmpstr(g_hash_table_lookup(matches, "asd"), ==, "hehe");
    balde_free_url_rule_match(m);
    g_hash_table_destroy(matches);
}


void
test_url_match_with_null_path(void)
{
    GHashTable *matches = NULL;
    balde_url_rule_match_t *m = balde_parse_url_rule("/", NULL);
    gboolean match = balde_url_match(NULL, m, &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 0);
    balde_free_url_rule_match(m);
    g_hash_table_destroy(matches);
}


void
test_url_match_with_empty_path(void)
{
    GHashTable *matches = NULL;
    balde_url_rule_match_t *m = balde_parse_url_rule("/", NULL);
    gboolean match = balde_url_match("", m, &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 0);
    balde_free_url_rule_match(m);
    g_hash_table_destroy(matches);
}


void
test_url_match_without_trailing_slash(void)
{
    GHashTable *matches = NULL;
    balde_url_rule_match_t *m = balde_parse_url_rule("/test/asd", NULL);
    gboolean match = balde_url_match("/test/asd", m, &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 0);
    balde_free_url_rule_match(m);
    g_hash_table_destroy(matches);
}


void
test_url_match_without_trailing_slash_with_variable(void)
{
    GHashTable *matches = NULL;
    balde_url_rule_match_t *m = balde_parse_url_rule("/test/<lol>", NULL);
    gboolean match = balde_url_match("/test/asd", m, &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 1);
    g_assert_cmpstr(g_hash_table_lookup(matches, "lol"), ==, "asd");
    balde_free_url_rule_match(m);
    g_hash_table_destroy(matches);
}


void
test_url_match_with_multiple_matches(void)
{
    GHashTable *matches = NULL;
    balde_url_rule_match_t *m = balde_parse_url_rule(
        "/test/<lol>/tset/<hehe1>/test/<xd>/", NULL);
    gboolean match = balde_url_match("/test/foo/tset/bar/test/baz/", m, &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 3);
    g_assert_cmpstr(g_hash_table_lookup(matches, "lol"), ==, "foo");
    g_assert_cmpstr(g_hash_table_lookup(matches, "hehe1"), ==, "bar");
    g_assert_cmpstr(g_hash_table_lookup(matches, "xd"), ==, "baz");
    balde_free_url_rule_match(m);
    g_hash_table_destroy(matches);
}


void
test_url_match_with_path(void)
{
    GHashTable *matches = NULL;
    balde_url_rule_match_t *m = balde_parse_url_rule("/foo/<path:p>/asd/", NULL);
    gboolean match = balde_url_match("/foo/guda/bola/arcoiro/asd/", m, &matches);
    g_assert(match);
    g_assert(g_hash_table_size(matches) == 1);
    g_assert_cmpstr(g_hash_table_lookup(matches, "p"), ==, "guda/bola/arcoiro");
    balde_free_url_rule_match(m);
    g_hash_table_destroy(matches);
}


void
test_url_no_match(void)
{
    GHashTable *matches = NULL;
    balde_url_rule_match_t *m = balde_parse_url_rule("/test/fool/", NULL);
    gboolean match = balde_url_match("/test/foo/", m, &matches);
    g_assert(!match);
    g_assert(matches == NULL);
    balde_free_url_rule_match(m);
}


void
test_url_no_match2(void)
{
    GHashTable *matches = NULL;
    balde_url_rule_match_t *m = balde_parse_url_rule("/test/", NULL);
    gboolean match = balde_url_match("/test/foo/", m, &matches);
    g_assert(!match);
    g_assert(matches == NULL);
    balde_free_url_rule_match(m);
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
test_url_rule_with_path(void)
{
    GSList *views = get_test_views();
    GHashTable *matches = NULL;
    gchar* endpoint = balde_dispatch_from_path(views, "/foo/bola/arcoiro/bar/",
        &matches);
    g_assert_cmpstr(endpoint, ==, "path");
    g_assert_cmpstr(g_hash_table_lookup(matches, "p"), ==, "bola/arcoiro");
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
    balde_url_rule_match_t *match;
    GError *error = NULL;
    match = balde_parse_url_rule("/foo/", &error);
    g_assert_cmpstr(g_regex_get_pattern(match->regex), ==, "^/foo/$");
    g_assert_cmpstr(match->pieces[0], ==, "/foo/");
    g_assert(match->pieces[1] == NULL);
    g_assert(match->args[0] == NULL);
    balde_free_url_rule_match(match);
    match = balde_parse_url_rule("/foo/<bar>/", &error);
    g_assert_cmpstr(g_regex_get_pattern(match->regex), ==,
        "^/foo/(?P<bar>[^/]+)/$");
    g_assert_cmpstr(match->pieces[0], ==, "/foo/");
    g_assert_cmpstr(match->pieces[1], ==, "/");
    g_assert(match->pieces[2] == NULL);
    g_assert_cmpstr(match->args[0], ==, "bar");
    g_assert(match->args[1] == NULL);
    balde_free_url_rule_match(match);
    match = balde_parse_url_rule("/foo/<bar>/<path:lol>/", &error);
    g_assert_cmpstr(g_regex_get_pattern(match->regex), ==,
        "^/foo/(?P<bar>[^/]+)/(?P<lol>[^/].*?)/$");
    g_assert_cmpstr(match->pieces[0], ==, "/foo/");
    g_assert_cmpstr(match->pieces[1], ==, "/");
    g_assert_cmpstr(match->pieces[2], ==, "/");
    g_assert(match->pieces[3] == NULL);
    g_assert_cmpstr(match->args[0], ==, "bar");
    g_assert_cmpstr(match->args[1], ==, "lol");
    g_assert(match->args[2] == NULL);
    balde_free_url_rule_match(match);
    match = balde_parse_url_rule("/foo/<bar>/<path:lol>/<path:baz>/<kkk>/",
        &error);
    g_assert_cmpstr(g_regex_get_pattern(match->regex), ==,
        "^/foo/(?P<bar>[^/]+)/(?P<lol>[^/].*?)/(?P<baz>[^/].*?)/(?P<kkk>[^/]+)/$");
    g_assert_cmpstr(match->pieces[0], ==, "/foo/");
    g_assert_cmpstr(match->pieces[1], ==, "/");
    g_assert_cmpstr(match->pieces[2], ==, "/");
    g_assert_cmpstr(match->pieces[3], ==, "/");
    g_assert_cmpstr(match->pieces[4], ==, "/");
    g_assert(match->pieces[5] == NULL);
    g_assert_cmpstr(match->args[0], ==, "bar");
    g_assert_cmpstr(match->args[1], ==, "lol");
    g_assert_cmpstr(match->args[2], ==, "baz");
    g_assert_cmpstr(match->args[3], ==, "kkk");
    g_assert(match->args[4] == NULL);
    balde_free_url_rule_match(match);
    match = balde_parse_url_rule("/foo/<path:foo_bar1>/", &error);
    g_assert_cmpstr(g_regex_get_pattern(match->regex), ==,
        "^/foo/(?P<foo_bar1>[^/].*?)/$");
    g_assert_cmpstr(match->pieces[0], ==, "/foo/");
    g_assert_cmpstr(match->pieces[1], ==, "/");
    g_assert(match->pieces[2] == NULL);
    g_assert_cmpstr(match->args[0], ==, "foo_bar1");
    g_assert(match->args[1] == NULL);
    balde_free_url_rule_match(match);
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
    g_test_add_func("/routing/url_match_with_path", test_url_match_with_path);
    g_test_add_func("/routing/url_no_match", test_url_no_match);
    g_test_add_func("/routing/url_no_match2", test_url_no_match2);
    g_test_add_func("/routing/url_rule_with_path", test_url_rule_with_path);
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
