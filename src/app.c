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
#include <stdlib.h>
#include "balde.h"
#include "app.h"
#include "cgi.h"
#include "exceptions.h"
#include "resources.h"
#include "routing.h"
#include "wrappers.h"

#ifdef BUILD_WEBSERVER
#include "httpd.h"
#endif

#ifdef BUILD_FASTCGI
#include <fcgiapp.h>
#include "fcgi.h"
#endif

balde_app_t*
balde_app_init(void)
{
    balde_app_t *app = g_new(balde_app_t, 1);
    app->priv = g_new(struct _balde_app_private_t, 1);
    app->priv->copy = FALSE;
    app->priv->views = NULL;
    app->priv->before_requests = NULL;
    app->priv->static_resources = NULL;
    app->priv->user_data = NULL;
    app->priv->config = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    app->error = NULL;
    balde_app_add_url_rule(app, "static", "/static/<path:file>", BALDE_HTTP_GET,
        balde_resource_view);
    return app;
}


balde_app_t*
balde_app_copy(balde_app_t *app)
{
    balde_app_t *copy = g_new(balde_app_t, 1);
    copy->error = NULL;
    copy->priv = g_new(struct _balde_app_private_t, 1);
    *(copy->priv) = *(app->priv);
    copy->priv->copy = TRUE;
    return copy;
}


G_LOCK_DEFINE_STATIC(config);

void
balde_app_set_config(balde_app_t *app, const gchar *name, const gchar *value)
{
    BALDE_APP_READ_ONLY(app);
    G_LOCK(config);
    g_hash_table_replace(app->priv->config, g_utf8_strdown(name, -1), g_strdup(value));
    G_UNLOCK(config);
}


void
balde_app_set_config_from_envvar(balde_app_t *app, const gchar *name,
    const gchar *env_name, gboolean silent)
{
    BALDE_APP_READ_ONLY(app);
    const gchar *value = g_getenv(env_name);
    if (value == NULL && !silent) {
        gchar *msg = g_strdup_printf("%s environment variable must be set",
            env_name);
        balde_abort_set_error_with_description(app, 500, msg);
        g_free(msg);
        return;
    }
    balde_app_set_config(app, name, value);
}


const gchar*
balde_app_get_config(balde_app_t *app, const gchar *name)
{
    gchar *tmp = g_utf8_strdown(name, -1);
    const gchar *rv = g_hash_table_lookup(app->priv->config, tmp);
    g_free(tmp);
    return rv;
}


void
balde_app_set_user_data(balde_app_t *app, void *user_data)
{
    BALDE_APP_READ_ONLY(app);
    app->priv->user_data = user_data;
}


void*
balde_app_get_user_data(balde_app_t *app)
{
    return app->priv->user_data;
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
    if (!app->priv->copy) {
        g_slist_free_full(app->priv->views, (GDestroyNotify) balde_app_free_views);
        g_slist_free(app->priv->before_requests);
        g_slist_free_full(app->priv->static_resources, (GDestroyNotify) balde_resource_free);
        g_hash_table_destroy(app->priv->config);
    }
    g_clear_error(&app->error);
    g_free(app->priv);
    g_free(app);
}


G_LOCK_DEFINE_STATIC(views);

void
balde_app_add_url_rule(balde_app_t *app, const gchar *endpoint, const gchar *rule,
    const balde_http_method_t method, balde_view_func_t view_func)
{
    BALDE_APP_READ_ONLY(app);
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
    G_LOCK(views);
    app->priv->views = g_slist_append(app->priv->views, view);
    G_UNLOCK(views);
}


G_LOCK_DEFINE_STATIC(before_requests);

void
balde_app_add_before_request(balde_app_t *app, balde_before_request_func_t hook_func)
{
    BALDE_APP_READ_ONLY(app);
    G_LOCK(before_requests);
    app->priv->before_requests = g_slist_append(app->priv->before_requests, hook_func);
    G_UNLOCK(before_requests);
}


balde_view_t*
balde_app_get_view_from_endpoint(balde_app_t *app, const gchar *endpoint)
{
    for (GSList *tmp = app->priv->views; tmp != NULL; tmp = g_slist_next(tmp)) {
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

static gboolean version = FALSE;

#ifdef BUILD_WEBSERVER
static gboolean runserver = FALSE;
static gchar *host = NULL;
static gint16 port = 8080;
static gint max_threads = 10;
#endif

static GOptionEntry entries[] =
{
    {"version", 0, 0, G_OPTION_ARG_NONE, &version,
        "Show balde's version number and exit.", NULL},

#ifdef BUILD_WEBSERVER
    {"runserver", 's', 0, G_OPTION_ARG_NONE, &runserver,
        "Run embedded server.", NULL},
    {"host", 't', 0, G_OPTION_ARG_STRING, &host,
        "Embedded server host. (default: 127.0.0.1)", "HOST"},
    {"port", 'p', 0, G_OPTION_ARG_INT, &port,
        "Embedded server port. (default: 8080)", "PORT"},
    {"max-threads", 'm', 0, G_OPTION_ARG_INT, &max_threads,
        "Max number of threads for embedded server. (default: 10)", "THREADS"},
#endif

    {NULL}
};


void
balde_app_run(balde_app_t *app, gint argc, gchar **argv)
{
    setlocale(LC_ALL, "");
    GError *err = NULL;
    GOptionContext *context = g_option_context_new("- a balde application ;-)");
    g_option_context_add_main_entries(context, entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &err)) {
        g_printerr("Option parsing failed: %s\n", err->message);
        exit(1);
    }
    if (version)
        g_printerr("%s\n", PACKAGE_STRING);

#ifdef BUILD_WEBSERVER
    else if (runserver)
        balde_httpd_run(app, host, port, max_threads);
#endif

#ifdef BUILD_FASTCGI
    else if (!FCGX_IsCGI()) {
        const gchar *threads_str = g_getenv("BALDE_FASTCGI_THREADS");
        guint64 threads = 1;
        if (threads_str != NULL && threads_str[0] != '\0')
            threads = g_ascii_strtoull(threads_str, NULL, 10);
        balde_fcgi_run(app, threads);
    }
#endif

    else if (g_getenv("REQUEST_METHOD") != NULL)
        balde_cgi_run(app);
    else {
        gchar *help = g_option_context_get_help(context, FALSE, NULL);
        g_printerr("%s", help);
        g_free(help);
    }
    g_option_context_free(context);

#ifdef BUILD_WEBSERVER
    g_free(host);
#endif

}


GString*
balde_app_main_loop(balde_app_t *app, balde_request_env_t *env,
    balde_response_render_t render, balde_http_exception_code_t *status_code)
{
    balde_request_t *request;
    balde_response_t *response;
    balde_response_t *error_response;
    gchar *endpoint;
    gboolean with_body = TRUE;
    GString *rv = NULL;

    // render startup error, if any
    if (app->error != NULL) {
        error_response = balde_make_response_from_exception(app->error);
        rv = render(error_response, with_body);
        if (status_code != NULL)
            *status_code = error_response->status_code;
        balde_response_free(error_response);
        return rv;
    }

    request = balde_make_request(app, env);

    for (GSList *tmp = app->priv->before_requests; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_before_request_func_t hook_func = tmp->data;
        hook_func(app, request);
    }

    balde_app_t *app_copy = balde_app_copy(app);

    with_body = ! (request->method & BALDE_HTTP_HEAD);

    // get the view
    endpoint = balde_dispatch_from_path(app_copy->priv->views, request->path,
        &(request->priv->view_args));
    if (endpoint == NULL) {  // no view found! :(
        balde_abort_set_error(app_copy, 404);
    }
    else {
        // validate http method
        balde_view_t *view = balde_app_get_view_from_endpoint(app_copy, endpoint);
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
                response = view->view_func(app_copy, request);
            }
        }
        // method not allowed
        else {
            balde_abort_set_error(app_copy, 405);
        }
        g_free(endpoint);
    }

    balde_request_free(request);

    if (app_copy->error != NULL) {
        error_response = balde_make_response_from_exception(app_copy->error);
        rv = render(error_response, with_body);
        if (status_code != NULL)
            *status_code = error_response->status_code;
        balde_response_free(error_response);
        balde_app_free(app_copy);
        return rv;
    }

    rv = render(response, with_body);
    if (status_code != NULL)
        *status_code = response->status_code;
    balde_response_free(response);
    balde_app_free(app_copy);

    return rv;
}
