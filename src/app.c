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
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include "balde.h"
#include "balde-private.h"
#include "app.h"
#include "cgi.h"
#include "httpd.h"
#include "exceptions.h"
#include "resources.h"
#include "routing.h"
#include "requests.h"
#include "responses.h"
#include "scgi.h"


static GLogLevelFlags
balde_get_log_level_flag_from_string(const gchar *level)
{
    if (level == NULL)
        return G_LOG_LEVEL_MESSAGE;

    gchar *level_str = g_ascii_strup(level, -1);

    GLogLevelFlags level_flag = G_LOG_LEVEL_MESSAGE;
    if (g_strcmp0(level_str, "CRITICAL") == 0)
        level_flag = G_LOG_LEVEL_CRITICAL;
    else if (g_strcmp0(level_str, "WARNING") == 0)
        level_flag = G_LOG_LEVEL_WARNING;
    else if (g_strcmp0(level_str, "MESSAGE") == 0)
        level_flag = G_LOG_LEVEL_MESSAGE;
    else if (g_strcmp0(level_str, "INFO") == 0)
        level_flag = G_LOG_LEVEL_INFO;
    else if (g_strcmp0(level_str, "DEBUG") == 0)
        level_flag = G_LOG_LEVEL_DEBUG;
    else
        g_printerr("ERROR: Invalid log level, defaulting to MESSAGE ...\n");

    g_free(level_str);
    return level_flag;
}


static void
balde_log_handler(const gchar *log_domain, GLogLevelFlags log_level,
    const gchar *message, gpointer user_data)
{
    GLogLevelFlags wanted_log_level = GPOINTER_TO_INT(user_data);
    if (log_level <= wanted_log_level) {
        const gchar *level_str = NULL;
        switch (log_level & G_LOG_LEVEL_MASK) {
            case G_LOG_LEVEL_ERROR:
                return;  // INVALID
            case G_LOG_LEVEL_CRITICAL:
                level_str = "CRITICAL";
                break;
            case G_LOG_LEVEL_WARNING:
                level_str = "WARNING";
                break;
            case G_LOG_LEVEL_MESSAGE:
                level_str = "MESSAGE";
                break;
            case G_LOG_LEVEL_INFO:
                level_str = "INFO";
                break;
            case G_LOG_LEVEL_DEBUG:
                level_str = "DEBUG";
                break;
        }
        fprintf(stderr, "%s: %s\n", level_str, message);
    }
}


BALDE_API balde_app_t*
balde_app_init(void)
{
    balde_app_t *app = g_new(balde_app_t, 1);
    app->priv = g_new(struct _balde_app_private_t, 1);
    app->priv->views = NULL;
    app->priv->before_requests = NULL;
    app->priv->static_resources = NULL;
    app->priv->user_data = NULL;
    app->priv->user_data_destroy_func = NULL;
    app->priv->config = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    app->copy = FALSE;
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
    copy->copy = TRUE;
    copy->priv = app->priv;
    return copy;
}


G_LOCK_DEFINE_STATIC(config);

BALDE_API void
balde_app_set_config(balde_app_t *app, const gchar *name, const gchar *value)
{
    BALDE_APP_READ_ONLY(app);
    G_LOCK(config);
    g_hash_table_replace(app->priv->config, g_utf8_strdown(name, -1), g_strdup(value));
    G_UNLOCK(config);
}


BALDE_API void
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


BALDE_API const gchar*
balde_app_get_config(balde_app_t *app, const gchar *name)
{
    gchar *tmp = g_utf8_strdown(name, -1);
    const gchar *rv = g_hash_table_lookup(app->priv->config, tmp);
    g_free(tmp);
    return rv;
}


G_LOCK_DEFINE_STATIC(user_data);

BALDE_API void
balde_app_set_user_data(balde_app_t *app, gpointer user_data)
{
    // when setting, if we have a destroy function, try to use it.
    balde_app_free_user_data(app);

    G_LOCK(user_data);
    app->priv->user_data = user_data;
    G_UNLOCK(user_data);
}


BALDE_API gpointer
balde_app_get_user_data(balde_app_t *app)
{
    return app->priv->user_data;
}


BALDE_API void
balde_app_set_user_data_destroy_func(balde_app_t *app, GDestroyNotify destroy_func)
{
    BALDE_APP_READ_ONLY(app);
    app->priv->user_data_destroy_func = destroy_func;
}


BALDE_API void
balde_app_free_user_data(balde_app_t *app)
{
    if (app->priv->user_data_destroy_func != NULL && app->priv->user_data != NULL) {
        G_LOCK(user_data);
        app->priv->user_data_destroy_func(app->priv->user_data);
        app->priv->user_data = NULL;
        G_UNLOCK(user_data);
    }
}


void
balde_app_free_views(balde_view_t *view)
{
    balde_free_url_rule_match(view->url_rule->match);
    g_free(view->url_rule);
    g_free(view);
}


BALDE_API void
balde_app_free(balde_app_t *app)
{
    if (app == NULL)
        return;
    if (!app->copy) {
        g_slist_free_full(app->priv->views, (GDestroyNotify) balde_app_free_views);
        g_slist_free_full(app->priv->before_requests, g_free);
        g_slist_free_full(app->priv->static_resources, (GDestroyNotify) balde_resource_free);
        g_hash_table_destroy(app->priv->config);
        balde_app_free_user_data(app);
        g_free(app->priv);
    }
    g_clear_error(&app->error);
    g_free(app);
}


G_LOCK_DEFINE_STATIC(views);

BALDE_API void
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

BALDE_API void
balde_app_add_before_request(balde_app_t *app, balde_before_request_func_t hook_func)
{
    BALDE_APP_READ_ONLY(app);
    balde_before_request_t *func = g_new(balde_before_request_t, 1);
    func->before_request_func = hook_func;
    G_LOCK(before_requests);
    app->priv->before_requests = g_slist_append(app->priv->before_requests, func);
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


BALDE_API gchar*
balde_app_url_for(balde_app_t *app, balde_request_t *request,
    const gchar *endpoint, gboolean external, ...)
{
    va_list params;
    va_start(params, external);
    gchar *rv = balde_app_url_forv(app, request, endpoint, params);
    va_end(params);
    return rv;
}


gchar*
balde_app_url_forv(balde_app_t *app, balde_request_t *request,
    const gchar *endpoint, va_list params)
{
    balde_view_t *view = balde_app_get_view_from_endpoint(app, endpoint);
    if (view == NULL)
        return NULL;
    const gchar *script_name = request->script_name;
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


static gboolean help = FALSE;
static gboolean version = FALSE;
static gchar *log_level = NULL;

static gboolean runserver = FALSE;
static gboolean runscgi = FALSE;

static GOptionEntry entries[] =
{
    {"help", 'h', 0, G_OPTION_ARG_NONE, &help,
        "Show help options", NULL},
    {"version", 'v', 0, G_OPTION_ARG_NONE, &version,
        "Show balde's version number and exit.", NULL},
    {"log-level", 'l', 0, G_OPTION_ARG_STRING, &log_level,
        "Logging level (CRITICAL, WARNING, MESSAGE, INFO, DEBUG). "
        "(default: MESSAGE)", "LEVEL"},
    {"runserver", 's', 0, G_OPTION_ARG_NONE, &runserver,
        "Run embedded HTTP server. NOT production ready!", NULL},
    {"runscgi", 'c', 0, G_OPTION_ARG_NONE, &runscgi, "Listen to SCGI socket.",
        NULL},
    {NULL}
};


static gchar *http_host = NULL;
static gint16 http_port = 8080;
static guint64 http_max_threads = 10;

static GOptionEntry entries_http[] =
{
    {"http-host", 0, 0, G_OPTION_ARG_STRING, &http_host,
        "Embedded HTTP server host. (default: 127.0.0.1)", "HOST"},
    {"http-port", 0, 0, G_OPTION_ARG_INT, &http_port,
        "Embedded HTTP server port. (default: 8080)", "PORT"},
    {"http-max-threads", 0, 0, G_OPTION_ARG_INT, &http_max_threads,
        "Embedded HTTP server max threads. (default: 10)", "THREADS"},
    {NULL}
};


static gchar *scgi_host = NULL;
static gint16 scgi_port = 9000;
static guint64 scgi_max_threads = 10;

static GOptionEntry entries_scgi[] =
{
    {"scgi-host", 0, 0, G_OPTION_ARG_STRING, &scgi_host,
        "Embedded SCGI server host. (default: 127.0.0.1)", "HOST"},
    {"scgi-port", 0, 0, G_OPTION_ARG_INT, &scgi_port,
        "Embedded SCGI server port. (default: 9000)", "PORT"},
    {"scgi-max-threads", 0, 0, G_OPTION_ARG_INT, &scgi_max_threads,
        "Embedded SCGI server max threads. (default: 10)", "THREADS"},
    {NULL}
};


BALDE_API void
balde_app_run(balde_app_t *app, gint argc, gchar **argv)
{
    setlocale(LC_ALL, "");
    GError *err = NULL;
    GOptionContext *context = g_option_context_new("- a balde application ;-)");
    g_option_context_add_main_entries(context, entries, NULL);

    GOptionGroup *http_group = g_option_group_new("http", "HTTP Options:",
        "Show HTTP help options", NULL, NULL);
    g_option_group_add_entries(http_group, entries_http);
    g_option_context_add_group(context, http_group);

    GOptionGroup *scgi_group = g_option_group_new("scgi", "SCGI Options:",
        "Show SCGI help options", NULL, NULL);
    g_option_group_add_entries(scgi_group, entries_scgi);
    g_option_context_add_group(context, scgi_group);

    g_option_context_set_help_enabled(context, FALSE);

    if (!g_option_context_parse(context, &argc, &argv, &err)) {
        g_printerr("Option parsing failed: %s\n", err->message);
        exit(1);
    }

    g_log_set_handler(BALDE_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL |
        G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO |
        G_LOG_LEVEL_DEBUG, balde_log_handler,
        GINT_TO_POINTER(balde_get_log_level_flag_from_string(log_level)));

    if (runserver && runscgi) {
        g_printerr("ERROR: --runserver conflicts with --runscgi\n");
        goto clean;
    }

    if (http_host != NULL)
        runserver = TRUE;

    if (scgi_host != NULL)
        runscgi = TRUE;

    if (help) {
        gchar *help_str = g_option_context_get_help(context, FALSE, NULL);
        g_print("%s", help_str);
        g_free(help_str);
    }
    else if (version) {
        g_printerr("%s\n", PACKAGE_STRING);
    }
    else if (runscgi) {
        balde_scgi_run(app, scgi_host, scgi_port, scgi_max_threads);
    }
    else if (runserver) {
        balde_httpd_run(app, http_host, http_port, http_max_threads);
    }
    else if (g_getenv("REQUEST_METHOD") != NULL) {
        balde_cgi_run(app);
    }
    else {
        gchar *help_str = g_option_context_get_help(context, FALSE, NULL);
        g_printerr("%s", help_str);
        g_free(help_str);
    }

clean:
    g_option_context_free(context);

    g_free(http_host);
    g_free(scgi_host);
    g_free(log_level);
}


GString*
balde_app_main_loop(balde_app_t *app, balde_request_env_t *env,
    balde_response_render_t render, balde_http_exception_code_t *status_code)
{
    balde_request_t *request = NULL;
    balde_response_t *response = NULL;
    balde_response_t *error_response = NULL;
    gchar *endpoint = NULL;
    gboolean with_body = TRUE;
    GString *rv = NULL;

    // render startup error, if any
    if (app->error != NULL) {
        error_response = balde_make_response_from_exception(app->error);
        rv = render(error_response, with_body);
        if (status_code != NULL)
            *status_code = error_response->status_code;
        balde_response_free(error_response);

        // free env, because it should be free'd by main loop and will not be
        // used anymore.
        balde_request_env_free(env);

        return rv;
    }

    request = balde_make_request(app, env);

    with_body = ! (request->method & BALDE_HTTP_HEAD);

    for (GSList *tmp = app->priv->before_requests; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_before_request_t *hook = tmp->data;
        hook->before_request_func(app, request);

        if (app->error != NULL) {
            error_response = balde_make_response_from_exception(app->error);
            rv = render(error_response, with_body);
            if (status_code != NULL)
                *status_code = error_response->status_code;
            balde_response_free(error_response);
            g_clear_error(&app->error);
            return rv;
        }
    }

    balde_app_t *app_copy = balde_app_copy(app);

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
        balde_response_free(response);
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
