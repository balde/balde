/**
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_APP_H
#define _BALDE_APP_H

#include <glib.h>
#include <balde/routing.h>
#include <balde/wrappers.h>

typedef struct {
    GSList *views;
    GHashTable *config;
    GError *error;
} balde_app_t;

typedef balde_response_t* (*balde_view_func_t) (balde_app_t*, balde_request_t*);

typedef struct {
    balde_url_rule_t *url_rule;
    balde_view_func_t view_func;
} balde_view_t;

balde_app_t* balde_app_init(void);
void balde_app_free_views(balde_view_t *view);
void balde_app_free(balde_app_t *app);
void balde_app_add_url_rule(balde_app_t *app, const gchar *endpoint,
    const gchar *rule, const balde_http_method_t method,
    balde_view_func_t view_func);
balde_view_t* balde_app_get_view_from_endpoint(balde_app_t *app, gchar *endpoint);
void balde_app_run(balde_app_t *app);

#endif /* _BALDE_APP_H */
