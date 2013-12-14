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
#include <locale.h>
#include <balde/app.h>
#include <balde/cgi.h>
#include <balde/exceptions.h>
#include <balde/routing.h>
#include <balde/wrappers.h>


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


balde_view_t*
balde_app_get_view_from_endpoint(balde_app_t *app, gchar *endpoint)
{
    for (GSList *tmp = app->views; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_view_t *view = tmp->data;
        if (0 == g_strcmp0(view->url_rule->endpoint, endpoint))
            return view;
    }
    return NULL;
}


void
balde_app_run(balde_app_t *app)
{
    setlocale(LC_ALL, "");
    g_set_print_handler(balde_stdout_handler);
    g_set_printerr_handler(balde_stderr_handler);

    balde_response_t *response;
    balde_response_t *error_response;
    GHashTable *matches;

BEGIN_LOOP

    // render error, if any
    error_response = balde_make_response_from_exception(app->error);
    if (error_response != NULL) {
        balde_response_print(error_response);
        balde_response_free(error_response);
        g_error_free(app->error);
        app->error = NULL;
        continue;
    }

    // get the view
    gchar *endpoint = balde_dispatch_from_path(app->views,
        (gchar*) g_getenv("PATH_INFO"), &matches);
    if (endpoint == NULL) {  // no view found! :(
        balde_abort(app, 404);
    }
    else {
        // run the view
        balde_view_t *view = balde_app_get_view_from_endpoint(app, endpoint);
        response = view->view_func(app, NULL);
    }

    // get errors
    error_response = balde_make_response_from_exception(app->error);
    if (error_response != NULL) {
        balde_response_print(error_response);
        balde_response_free(error_response);
        g_error_free(app->error);
        app->error = NULL;
        continue;
    }

    balde_response_print(response);
    balde_response_free(response);

END_LOOP

    balde_app_free(app);
}
