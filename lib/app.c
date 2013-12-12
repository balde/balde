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


balde_app_t*
balde_app_init(void)
{
    balde_app_t *app = g_new(balde_app_t, 1);
    app->views = NULL;
    // TODO: load config from somewhere
    app->config = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    app->error = NULL;
    return app;
}


void
balde_app_free_views(balde_view_t *view)
{
    g_free(view->url_rule);
    g_free(view);
}


void
balde_app_free(balde_app_t *app)
{
    if (app->views != NULL)
        g_slist_free_full(app->views, (GDestroyNotify) balde_app_free_views);
    g_hash_table_destroy(app->config);
    if (app->error != NULL)
        g_error_free(app->error);
    g_free(app);
}


void
balde_app_add_url_rule(balde_app_t *app, const gchar *endpoint, const gchar *rule,
    balde_view_func_t view_func)
{
    balde_view_t *view = g_new(balde_view_t, 1);
    view->url_rule = g_new(balde_url_rule_t, 1);
    view->url_rule->endpoint = endpoint;
    view->url_rule->rule = rule;
    view->view_func = view_func;
    app->views = g_slist_append(app->views, view);
}
