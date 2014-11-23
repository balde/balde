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
 * This is an opaque structure that stores everything related to the
 * application context. It should stay loaded in memory during all the
 * application life cycle, and reused by all the requests.
 */
typedef struct _balde_app_t balde_app_t;

/** View type definition
 *
 * Each view should accept the application context and the request context,
 * and return a response context.
 */
typedef balde_response_t* (*balde_view_func_t) (balde_app_t*, balde_request_t*);

/** "Before request" hook type definition
 *
 * Each hook should accept the application context and the request context.
 *
 */
typedef void (*balde_before_request_func_t) (balde_app_t*, balde_request_t*);

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


/** Sets a configuration parameter with the value of an environment variable.
 *
 * The parameter name is case-insensitive, but the parameter env_name is case
 * sensitive.
 *
 * If the parameter silent is false, this function will raise an error if the
 * environment variable isn't found or is NULL.
 */
void balde_app_set_config_from_envvar(balde_app_t *app, const gchar *name,
    const gchar *env_name, gboolean silent);


/** Gets a configuration parameter.
 *
 * The parameter name is case-insensitive.
 */
const gchar* balde_app_get_config(balde_app_t *app, const gchar *name);


/** Sets user data.
 *
 * The memory needed by the user provided data should be allocated previously,
 * and free'd afterwards.
 */
void balde_app_set_user_data(balde_app_t *app, void *user_data);


/** Gets user data. */
void* balde_app_get_user_data(balde_app_t *app);

/** Gets current error.
 *
 * This function returns the error object of the current context, if any, or
 * NULL.
 *
 * If the function is called from the context of a view, it will return the
 * error from the request context, otherwise it will return the error from the
 * application context.
 *
 * Errors are usually non-2XX HTTP status codes.
 */
const GError* balde_app_get_error(balde_app_t *app);

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

/** Adds a "before request" hook to the balde application */
void balde_app_add_before_request(balde_app_t *app,
    balde_before_request_func_t hook_func);

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
void balde_app_run(balde_app_t *app, gint argc, gchar **argv);

#endif /* _BALDE_APP_H */
