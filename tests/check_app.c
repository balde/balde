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
#include <balde/wrappers.h>


static guint i = 0;


void
test_app_init(void)
{
    balde_app_t *app = balde_app_init();
    g_assert(app != NULL);
    g_assert(app->config != NULL);
    g_assert(g_hash_table_size(app->config) == 0);
    g_assert(app->views == NULL);
    g_assert(app->error == NULL);
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
    balde_app_add_url_rule(app, "arcoiro", "/arcoiro/", &arcoiro_view);
    g_assert(g_slist_length(app->views) == 1);
    balde_view_t *view = app->views->data;
    g_assert(view != NULL);
    g_assert(view->url_rule != NULL);
    g_assert_cmpstr(view->url_rule->endpoint, ==, "arcoiro");
    g_assert_cmpstr(view->url_rule->rule, ==, "/arcoiro/");
    view->view_func(app, NULL);
    g_assert(i == 1);
    balde_app_free(app);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/app/init", test_app_init);
    return g_test_run();
}
