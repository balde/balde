/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_APP_H
#define _BALDE_APP_H

#include <glib.h>
#include <balde/wrappers.h>

/** Application related definitions.
 *
 * \file balde/app.h
 */

/** balde application context
 *
 * This struct stores everything related to the application context. It stays
 * loaded in memory during all the application life time, and is reused by all
 * the requests.
 */
typedef struct {

    /**
     * A GLib singly linked list that stores the application views. Do not
     * touch this, use the balde_app_add_url_rule() function instead!
     */
    GSList *views;

    /**
     * A GLib singly linked list that stores the static resources. Do not touch
     * it!
     */
    GSList *static_resources;

    /**
     * A GLib hash table that stores the application settings. Do not touch
     * this, use the balde_app_set_config() and balde_app_get_config()
     * functions instead!
     */
    GHashTable *config;

    /**
     * A GLib error instance. Do not touch this manually!
     */
    GError *error;

    /**
     * User data to be attached to the application context.
     */
    gpointer user_data;

} balde_app_t;

/** View type definition
 *
 * Each view should accept the application context and the request context,
 * and return a response context.
 */
typedef balde_response_t* (*balde_view_func_t) (balde_app_t*, balde_request_t*);

/** Initializes the application context
 *
 * This function allocates memory for the application context.
 */
balde_app_t* balde_app_init(void);


/** Sets a configuration parameter.
 *
 * The parameter name is case-insensitive.
 */
void balde_app_set_config(balde_app_t *app, const gchar *name, const gchar *value);


/** Gets a configuration parameter.
 *
 * The parameter name is case-insensitive.
 */
const gchar* balde_app_get_config(balde_app_t *app, const gchar *name);


/** Free application context memory.
 *
 * This function will clean and free all memory used by the application context.
 */
void balde_app_free(balde_app_t *app);

/** Adds a view to the balde application
 *
 * The endpoint should be unique, and multiple methods can be provided with pipes.
 */
void balde_app_add_url_rule(balde_app_t *app, const gchar *endpoint,
    const gchar *rule, const balde_http_method_t method,
    balde_view_func_t view_func);

/** Helper function to get the URL for a given endpoint.
 *
 * You should pass the exact number of parameters expected by the view's URL
 * rule.
 *
 * The 'external' parameter isn't implemented yet, but is planned on the API.
 * If set to TRUE, the function will return an absolute URL.
 */
gchar* balde_app_url_for(balde_app_t *app, const gchar *endpoint, gboolean external,
    ...);

/** Application main loop.
 *
 * This function does everything needed to run the registered views and dispatch
 * requests.
 */
void balde_app_run(balde_app_t *app);

#endif /* _BALDE_APP_H */
