/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_APP_PRIVATE_H
#define _BALDE_APP_PRIVATE_H

#include <glib.h>
#include <balde/app.h>
#include <balde/exceptions.h>
#include <balde/routing-private.h>
#include <balde/wrappers.h>
#include <balde/wrappers-private.h>

typedef struct {
    balde_url_rule_t *url_rule;
    balde_view_func_t view_func;
} balde_view_t;

typedef GString* (*balde_response_render_t) (balde_response_t*, const gboolean);

void balde_app_free_views(balde_view_t *view);
balde_view_t* balde_app_get_view_from_endpoint(balde_app_t *app,
    const gchar *endpoint);
gchar* balde_app_url_forv(balde_app_t *app, const gchar *endpoint, va_list params);
GString* balde_app_main_loop(balde_app_t *app, balde_request_env_t *env,
    balde_response_render_t render, balde_http_exception_code_t *status_code);

#endif /* _BALDE_APP_PRIVATE_H */
