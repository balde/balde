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
#include <balde/wrappers.h>
#include <balde/wrappers-private.h>


static guint i = 0;


void
test_app_init(void)
{
    balde_app_t *app = balde_app_init();
    g_assert(app != NULL);
    g_assert(app->config != NULL);
    g_assert(g_hash_table_size(app->config) == 0);
    g_assert(app->views != NULL);
    g_assert(app->views->next == NULL);
    g_assert(app->error == NULL);
    balde_app_free(app);
}


void
test_app_set_config(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_set_config(app, "BolA", "guda");
    g_assert(g_hash_table_size(app->config) == 1);
    g_assert_cmpstr(g_hash_table_lookup(app->config, "bola"), ==, "guda");
    balde_app_free(app);
}


void
test_app_get_config(void)
{
    balde_app_t *app = balde_app_init();
    g_hash_table_replace(app->config, g_strdup("bola"), g_strdup("guda"));
    g_assert_cmpstr(balde_app_get_config(app, "BoLa"), ==, "guda");
    balde_app_free(app);
}


balde_response_t*
arcoiro_view(balde_app_t *app, balde_request_t *req)
{
    i = 1;
    g_assert(app != NULL);
    return NULL;
}


void
test_app_add_url_rule(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_add_url_rule(app, "arcoiro", "/arcoiro/", BALDE_HTTP_POST,
        arcoiro_view);
    g_assert(g_slist_length(app->views) == 2);
    balde_view_t *view = app->views->next->data;
    g_assert(view != NULL);
    g_assert(view->url_rule != NULL);
    g_assert_cmpstr(view->url_rule->endpoint, ==, "arcoiro");
    g_assert_cmpstr(view->url_rule->rule, ==, "/arcoiro/");
    g_assert_cmpstr(g_regex_get_pattern(view->url_rule->match->regex), ==,
        "^/arcoiro/$");
    g_assert(view->url_rule->method & BALDE_HTTP_POST);
    g_assert(view->url_rule->method & BALDE_HTTP_OPTIONS);
    g_assert(!(view->url_rule->method & BALDE_HTTP_GET));
    g_assert(!(view->url_rule->method & BALDE_HTTP_HEAD));
    g_assert(!(view->url_rule->method & BALDE_HTTP_PUT));
    g_assert(!(view->url_rule->method & BALDE_HTTP_DELETE));
    i = 0;
    view->view_func(app, NULL);
    g_assert(i == 1);
    balde_app_free(app);
}


void
test_app_get_view_from_endpoint(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_add_url_rule(app, "arcoiro", "/arcoiro/", BALDE_HTTP_GET,
        arcoiro_view);
    balde_view_t *view = balde_app_get_view_from_endpoint(app, "arcoiro");
    g_assert(view != NULL);
    g_assert(view->url_rule != NULL);
    g_assert_cmpstr(view->url_rule->endpoint, ==, "arcoiro");
    g_assert_cmpstr(view->url_rule->rule, ==, "/arcoiro/");
    g_assert_cmpstr(g_regex_get_pattern(view->url_rule->match->regex), ==,
        "^/arcoiro/$");
    g_assert(view->url_rule->method & BALDE_HTTP_GET);
    g_assert(view->url_rule->method & BALDE_HTTP_HEAD);
    g_assert(view->url_rule->method & BALDE_HTTP_OPTIONS);
    g_assert(!(view->url_rule->method & BALDE_HTTP_POST));
    g_assert(!(view->url_rule->method & BALDE_HTTP_PUT));
    g_assert(!(view->url_rule->method & BALDE_HTTP_DELETE));
    i = 0;
    view->view_func(app, NULL);
    g_assert(i == 1);
    balde_app_free(app);
}


void
test_app_get_view_from_endpoint_not_found(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_add_url_rule(app, "arcoiro", "/arcoiro/", BALDE_HTTP_GET,
        arcoiro_view);
    balde_view_t *view = balde_app_get_view_from_endpoint(app, "bola");
    g_assert(view == NULL);
    balde_app_free(app);
}


void
test_app_url_for(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_add_url_rule(app, "arcoiro", "/arcoiro/<bola>/<guda>/",
        BALDE_HTTP_GET, arcoiro_view);
    balde_app_add_url_rule(app, "arcoiro2", "/arco/<iro>", BALDE_HTTP_GET,
        arcoiro_view);
    gchar *url = balde_app_url_for(app, "arcoiro", FALSE, "chunda", "guto");
    g_assert_cmpstr(url, ==, "/arcoiro/chunda/guto/");
    g_free(url);
    url = balde_app_url_for(app, "arcoiro2", FALSE, "bola");
    g_assert_cmpstr(url, ==, "/arco/bola");
    g_free(url);
    url = balde_app_url_for(app, "arcoiro2", FALSE, "bo\"la");
    g_assert_cmpstr(url, ==, "/arco/bo%22la");
    g_free(url);
    url = balde_app_url_for(app, "static", FALSE, "foo/jquery-min.js");
    g_assert_cmpstr(url, ==, "/static/foo/jquery-min.js");
    g_free(url);
    balde_app_free(app);
}


void
test_app_url_for_with_script_name(void)
{
    g_setenv("SCRIPT_NAME", "/foo/bar", TRUE);
    balde_app_t *app = balde_app_init();
    balde_app_add_url_rule(app, "arcoiro", "/arcoiro/<bola>/<guda>/",
        BALDE_HTTP_GET, arcoiro_view);
    balde_app_add_url_rule(app, "arcoiro2", "/arco/<iro>", BALDE_HTTP_GET,
        arcoiro_view);
    gchar *url = balde_app_url_for(app, "arcoiro", FALSE, "chunda", "guto");
    g_assert_cmpstr(url, ==, "/foo/bar/arcoiro/chunda/guto/");
    g_free(url);
    url = balde_app_url_for(app, "arcoiro2", FALSE, "bola");
    g_assert_cmpstr(url, ==, "/foo/bar/arco/bola");
    g_free(url);
    url = balde_app_url_for(app, "static", FALSE, "foo/jquery-min.js");
    g_assert_cmpstr(url, ==, "/foo/bar/static/foo/jquery-min.js");
    g_free(url);
    balde_app_free(app);
    g_unsetenv("SCRIPT_NAME");
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/app/init", test_app_init);
    g_test_add_func("/app/set_config", test_app_set_config);
    g_test_add_func("/app/get_config", test_app_get_config);
    g_test_add_func("/app/add_url_rule", test_app_add_url_rule);
    g_test_add_func("/app/get_view_from_endpoint",
        test_app_get_view_from_endpoint);
    g_test_add_func("/app/get_view_from_endpoint_not_found",
        test_app_get_view_from_endpoint_not_found);
    g_test_add_func("/app/url_for", test_app_url_for);
    g_test_add_func("/app/url_for_with_script_name",
        test_app_url_for_with_script_name);
    return g_test_run();
}
