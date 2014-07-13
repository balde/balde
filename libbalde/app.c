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
#include <locale.h>
#include <balde/app.h>
#include <balde/app-private.h>
#include <balde/cgi-private.h>
#include <balde/exceptions.h>
#include <balde/exceptions-private.h>
#include <balde/resources-private.h>
#include <balde/routing.h>
#include <balde/routing-private.h>
#include <balde/wrappers.h>
#include <balde/wrappers-private.h>


balde_app_t*
balde_app_init(void)
{
    balde_app_t *app = g_new(balde_app_t, 1);
    app->views = NULL;
    app->static_resources = NULL;
    app->config = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    app->error = NULL;
    balde_app_add_url_rule(app, "static", "/static/<path:file>", BALDE_HTTP_GET,
        balde_resource_view);
    return app;
}


void
balde_app_set_config(balde_app_t *app, const gchar *name, const gchar *value)
{
    g_hash_table_replace(app->config, g_utf8_strdown(name, -1), g_strdup(value));
}


const gchar*
balde_app_get_config(balde_app_t *app, const gchar *name)
{
    gchar *tmp = g_utf8_strdown(name, -1);
    const gchar *rv = g_hash_table_lookup(app->config, tmp);
    g_free(tmp);
    return rv;
}


void
balde_app_free_views(balde_view_t *view)
{
    balde_free_url_rule_match(view->url_rule->match);
    g_free(view->url_rule);
    g_free(view);
}


void
balde_app_free(balde_app_t *app)
{
    g_slist_free_full(app->views, (GDestroyNotify) balde_app_free_views);
    g_slist_free_full(app->static_resources, (GDestroyNotify) balde_resource_free);
    g_hash_table_destroy(app->config);
    g_clear_error(&app->error);
    g_free(app);
}


void
balde_app_add_url_rule(balde_app_t *app, const gchar *endpoint, const gchar *rule,
    const balde_http_method_t method, balde_view_func_t view_func)
{
    GError *tmp_error = NULL;
    balde_view_t *view = g_new(balde_view_t, 1);
    view->url_rule = g_new(balde_url_rule_t, 1);
    view->url_rule->endpoint = endpoint;
    view->url_rule->rule = rule;
    view->url_rule->match = balde_parse_url_rule(view->url_rule->rule, &tmp_error);
    if (tmp_error != NULL) {
        g_propagate_error(&(app->error), tmp_error);
        balde_app_free_views(view);
        return;
    }
    view->url_rule->method = method | BALDE_HTTP_OPTIONS;
    if (view->url_rule->method & BALDE_HTTP_GET)
        view->url_rule->method |= BALDE_HTTP_HEAD;
    view->view_func = view_func;
    app->views = g_slist_append(app->views, view);
}


balde_view_t*
balde_app_get_view_from_endpoint(balde_app_t *app, const gchar *endpoint)
{
    for (GSList *tmp = app->views; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_view_t *view = tmp->data;
        if (0 == g_strcmp0(view->url_rule->endpoint, endpoint))
            return view;
    }
    return NULL;
}


gchar*
balde_app_url_for(balde_app_t *app, const gchar *endpoint, gboolean external, ...)
{
    va_list params;
    va_start(params, external);
    gchar *rv = balde_app_url_forv(app, endpoint, params);
    va_end(params);
    return rv;
}


gchar*
balde_app_url_forv(balde_app_t *app, const gchar *endpoint, va_list params)
{
    balde_view_t *view = balde_app_get_view_from_endpoint(app, endpoint);
    if (view == NULL)
        return NULL;
    const gchar *script_name = g_getenv("SCRIPT_NAME");
    GString *p = g_string_new(script_name == NULL ? "" : script_name);
    for (guint i = 0; view->url_rule->match->pieces[i] != NULL; i++) {
        g_string_append(p, view->url_rule->match->pieces[i]);
        if (view->url_rule->match->pieces[i + 1] != NULL)
            g_string_append(p, va_arg(params, const gchar*));
    }
    gchar *tmp = g_string_free(p, FALSE);
    gchar *rv = g_uri_escape_string(tmp, "/:", TRUE);
    g_free(tmp);
    return rv;
}


/**
 * \example hello.c
 *
 * A hello world!
 */

void
balde_app_run(balde_app_t *app)
{
    setlocale(LC_ALL, "");
    g_set_printerr_handler(balde_stderr_handler);

BEGIN_LOOP

    balde_app_main_loop(app);

END_LOOP

}


void
balde_app_main_loop(balde_app_t *app)
{
    balde_request_t *request;
    balde_response_t *response;
    balde_response_t *error_response;
    gchar *endpoint;
    gboolean with_body = TRUE;

    // render startup error, if any
    if (app->error != NULL) {
        error_response = balde_make_response_from_exception(app->error);
        balde_response_print(error_response, with_body);
        balde_response_free(error_response);
        return;
    }

    request = balde_make_request(app);
    with_body = ! (request->method & BALDE_HTTP_HEAD);

    // get the view
    endpoint = balde_dispatch_from_path(app->views, request->path,
        &(request->view_args));
    if (endpoint == NULL) {  // no view found! :(
        balde_abort_set_error(app, 404);
    }
    else {
        // validate http method
        balde_view_t *view = balde_app_get_view_from_endpoint(app, endpoint);
        if (request->method & view->url_rule->method) {
            // answer OPTIONS automatically
            if (request->method == BALDE_HTTP_OPTIONS) {
                response = balde_make_response("");
                gchar *allow = balde_list_allowed_methods(view->url_rule->method);
                balde_response_set_header(response, "Allow", allow);
                g_free(allow);
            }
            // run the view
            else {
                response = view->view_func(app, request);
            }
        }
        // method not allowed
        else {
            balde_abort_set_error(app, 405);
        }
        g_free(endpoint);
    }

    balde_request_free(request);

    if (app->error != NULL) {
        error_response = balde_make_response_from_exception(app->error);
        balde_response_print(error_response, with_body);
        balde_response_free(error_response);
        g_clear_error(&app->error);
        return;
    }

    balde_response_print(response, with_body);
    balde_response_free(response);
}
