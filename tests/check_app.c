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
#include "../src/balde.h"
#include "../src/app.h"
#include "../src/requests.h"
#include "../src/responses.h"


static guint i = 0;


void
test_app_init(void)
{
    balde_app_t *app = balde_app_init();
    g_assert(app != NULL);
    g_assert(app->priv != NULL);
    g_assert(app->priv->config != NULL);
    g_assert(g_hash_table_size(app->priv->config) == 0);
    g_assert(app->priv->views != NULL);
    g_assert(app->priv->views->next == NULL);
    g_assert(app->error == NULL);
    balde_app_free(app);
}


void
test_app_copy(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_t *copy = balde_app_copy(app);
    g_assert(!app->copy);
    g_assert(copy->copy);
    balde_abort_set_error(app, 404);
    g_assert(app->error != NULL);
    g_assert(copy->error == NULL);
    g_assert(app->priv->views != NULL);
    g_assert(app->priv->views == copy->priv->views);
    g_assert(app->priv->config != NULL);
    g_assert(app->priv->config == copy->priv->config);
    g_assert(app->priv->user_data == NULL);
    g_assert(copy->priv->user_data == NULL);
    copy->priv->user_data = GINT_TO_POINTER(10);
    g_assert_cmpint(GPOINTER_TO_INT(copy->priv->user_data), ==, 10);
    g_assert_cmpint(GPOINTER_TO_INT(app->priv->user_data), ==, 10);
    balde_app_free(app);
    balde_app_free(copy);
}


void
test_app_set_config(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_set_config(app, "BolA", "guda");
    g_assert(g_hash_table_size(app->priv->config) == 1);
    g_assert_cmpstr(g_hash_table_lookup(app->priv->config, "bola"), ==, "guda");
    balde_app_free(app);
}


void
test_app_set_config_from_envvar(void)
{
    g_setenv("BOLA_CONFIG_ENVVAR", "guda", TRUE);
    balde_app_t *app = balde_app_init();
    balde_app_set_config_from_envvar(app, "foo", "BOLA_CONFIG_ENVVAR", FALSE);
    g_assert(g_hash_table_size(app->priv->config) == 1);
    g_assert_cmpstr(g_hash_table_lookup(app->priv->config, "foo"), ==, "guda");
    balde_app_free(app);
}


void
test_app_set_config_from_envvar_not_found(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_set_config_from_envvar(app, "foo", "BOLA_CONFIG_ENVVAR2", FALSE);
    g_assert(app->error != NULL);
    g_assert_cmpstr(app->error->message, ==,
        "The server encountered an internal error and was unable to complete "
        "your request. Either the server is overloaded or there is an error in "
        "the application.\n"
        "\n"
        "BOLA_CONFIG_ENVVAR2 environment variable must be set");
    g_assert(g_hash_table_size(app->priv->config) == 0);
    balde_app_free(app);
}


void
test_app_set_config_from_envvar_not_found_silent(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_set_config_from_envvar(app, "foo", "BOLA_CONFIG_ENVVAR3", TRUE);
    g_assert(app->error == NULL);
    g_assert(g_hash_table_size(app->priv->config) == 1);
    g_assert(g_hash_table_lookup(app->priv->config, "foo") == NULL);
    balde_app_free(app);
}


void
test_app_get_config(void)
{
    balde_app_t *app = balde_app_init();
    g_hash_table_replace(app->priv->config, g_strdup("bola"), g_strdup("guda"));
    g_assert_cmpstr(balde_app_get_config(app, "BoLa"), ==, "guda");
    balde_app_free(app);
}


void
test_app_set_user_data(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_set_user_data(app, GINT_TO_POINTER(10));
    g_assert_cmpint(GPOINTER_TO_INT(app->priv->user_data), ==, 10);
    balde_app_free(app);
}


void
test_app_get_user_data(void)
{
    balde_app_t *app = balde_app_init();
    app->priv->user_data = GINT_TO_POINTER(10);
    g_assert_cmpint(GPOINTER_TO_INT(balde_app_get_user_data(app)), ==, 10);
    app->priv->user_data = GINT_TO_POINTER(20);
    g_assert_cmpint(GPOINTER_TO_INT(balde_app_get_user_data(app)), ==, 20);
    balde_app_free(app);
}


void
test_app_set_user_data_with_destroyer(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_set_user_data_destroy_func(app, g_free);
    balde_app_set_user_data(app, g_strdup("bola"));
    g_assert_cmpstr(balde_app_get_user_data(app), ==, "bola");
    balde_app_set_user_data(app, g_strdup("chunda"));
    g_assert_cmpstr(balde_app_get_user_data(app), ==, "chunda");
    balde_app_free_user_data(app);
    balde_app_free(app);
}


void
test_app_free_user_data(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_set_user_data_destroy_func(app, g_free);
    balde_app_set_user_data(app, g_strdup("bola"));
    g_assert_cmpstr(balde_app_get_user_data(app), ==, "bola");
    balde_app_free_user_data(app);
    g_assert(balde_app_get_user_data(app) == NULL);
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
    g_assert(g_slist_length(app->priv->views) == 2);
    balde_view_t *view = app->priv->views->next->data;
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
arcoiro_hook(balde_app_t *app, balde_request_t *req)
{
    i = 1;
    g_assert(app != NULL);
}


void
test_app_add_before_request(void)
{
    balde_app_t *app = balde_app_init();
    balde_app_add_before_request(app, arcoiro_hook);
    g_assert(app->priv->before_requests != NULL);
    g_assert(app->priv->before_requests->next == NULL);
    i = 0;
    balde_before_request_t *hook = app->priv->before_requests->data;
    g_assert(hook != NULL);
    g_assert(hook->before_request_func == arcoiro_hook);
    hook->before_request_func(app, NULL);
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
    g_setenv("PATH_INFO", "/", TRUE);
    g_setenv("REQUEST_METHOD", "GET", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    balde_app_add_url_rule(app, "arcoiro", "/arcoiro/<bola>/<guda>/",
        BALDE_HTTP_GET, arcoiro_view);
    balde_app_add_url_rule(app, "arcoiro2", "/arco/<iro>", BALDE_HTTP_GET,
        arcoiro_view);
    gchar *url = balde_app_url_for(app, request, "arcoiro", FALSE, "chunda", "guto");
    g_assert_cmpstr(url, ==, "/arcoiro/chunda/guto/");
    g_free(url);
    url = balde_app_url_for(app, request, "arcoiro2", FALSE, "bola");
    g_assert_cmpstr(url, ==, "/arco/bola");
    g_free(url);
    url = balde_app_url_for(app, request, "arcoiro2", FALSE, "bo\"la");
    g_assert_cmpstr(url, ==, "/arco/bo%22la");
    g_free(url);
    url = balde_app_url_for(app, request, "static", FALSE, "foo/jquery-min.js");
    g_assert_cmpstr(url, ==, "/static/foo/jquery-min.js");
    g_free(url);
    balde_request_free(request);
    balde_app_free(app);
    g_unsetenv("REQUEST_METHOD");
    g_unsetenv("PATH_INFO");
}


void
test_app_url_for_with_script_name(void)
{
    g_setenv("PATH_INFO", "/", TRUE);
    g_setenv("REQUEST_METHOD", "GET", TRUE);
    g_setenv("SCRIPT_NAME", "/foo/bar", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    balde_app_add_url_rule(app, "arcoiro", "/arcoiro/<bola>/<guda>/",
        BALDE_HTTP_GET, arcoiro_view);
    balde_app_add_url_rule(app, "arcoiro2", "/arco/<iro>", BALDE_HTTP_GET,
        arcoiro_view);
    gchar *url = balde_app_url_for(app, request, "arcoiro", FALSE, "chunda", "guto");
    g_assert_cmpstr(url, ==, "/foo/bar/arcoiro/chunda/guto/");
    g_free(url);
    url = balde_app_url_for(app, request, "arcoiro2", FALSE, "bola");
    g_assert_cmpstr(url, ==, "/foo/bar/arco/bola");
    g_free(url);
    url = balde_app_url_for(app, request, "static", FALSE, "foo/jquery-min.js");
    g_assert_cmpstr(url, ==, "/foo/bar/static/foo/jquery-min.js");
    g_free(url);
    balde_request_free(request);
    balde_app_free(app);
    g_unsetenv("SCRIPT_NAME");
    g_unsetenv("REQUEST_METHOD");
    g_unsetenv("PATH_INFO");
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/app/init", test_app_init);
    g_test_add_func("/app/copy", test_app_copy);
    g_test_add_func("/app/set_config", test_app_set_config);
    g_test_add_func("/app/set_config_from_envvar",
        test_app_set_config_from_envvar);
    g_test_add_func("/app/set_config_from_envvar_not_found",
        test_app_set_config_from_envvar_not_found);
    g_test_add_func("/app/set_config_from_envvar_not_found_silent",
        test_app_set_config_from_envvar_not_found_silent);
    g_test_add_func("/app/get_config", test_app_get_config);
    g_test_add_func("/app/set_user_data", test_app_set_user_data);
    g_test_add_func("/app/get_user_data", test_app_get_user_data);
    g_test_add_func("/app/set_user_data_with_destroyer",
        test_app_set_user_data_with_destroyer);
    g_test_add_func("/app/free_user_data", test_app_free_user_data);
    g_test_add_func("/app/add_url_rule", test_app_add_url_rule);
    g_test_add_func("/app/add_before_request",
        test_app_add_before_request);
    g_test_add_func("/app/get_view_from_endpoint",
        test_app_get_view_from_endpoint);
    g_test_add_func("/app/get_view_from_endpoint_not_found",
        test_app_get_view_from_endpoint_not_found);
    g_test_add_func("/app/url_for", test_app_url_for);
    g_test_add_func("/app/url_for_with_script_name",
        test_app_url_for_with_script_name);
    return g_test_run();
}
