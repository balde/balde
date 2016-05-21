/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_APP_PRIVATE_H
#define _BALDE_APP_PRIVATE_H

#include <glib.h>
#include "balde.h"
#include "routing.h"
#include "requests.h"
#include "responses.h"

#define BALDE_APP_READ_ONLY(app)                                            \
    if ((app)->copy)                                                        \
        g_error(                                                            \
            "You called `%s()' from a view. This is unsupported! You may "  \
            "want to move this code to a \"before request\" hook.",         \
            __FUNCTION__)

struct _balde_app_private_t {
    GSList *views;
    GSList *before_requests;
    GSList *static_resources;
    GHashTable *config;
    gpointer user_data;
    GDestroyNotify user_data_destroy_func;
};

typedef struct {
    balde_url_rule_t *url_rule;
    balde_view_func_t view_func;
} balde_view_t;

typedef struct {
    balde_before_request_func_t before_request_func;
} balde_before_request_t;

typedef GString* (*balde_response_render_t) (balde_response_t*, const gboolean);

balde_app_t* balde_app_copy(balde_app_t *app);
void balde_app_free_views(balde_view_t *view);
balde_view_t* balde_app_get_view_from_endpoint(balde_app_t *app,
    const gchar *endpoint);
gchar* balde_app_url_forv(balde_app_t *app, balde_request_t *request,
    const gchar *endpoint, va_list params);
GString* balde_app_main_loop(balde_app_t *app, balde_request_env_t *env,
    balde_response_render_t render, balde_http_exception_code_t *status_code);

#endif /* _BALDE_APP_PRIVATE_H */
