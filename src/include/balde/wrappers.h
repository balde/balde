/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_WRAPPERS_H
#define _BALDE_WRAPPERS_H

#include <glib.h>
#include <balde/routing.h>

/** Request/Response context related definitions
 *
 * \file balde/wrappers.h
 */

/** balde request context
 *
 * This struct stores everything related to the request context. It stays
 * loaded in memory during all the request life time, and is initialized
 * automatically by the application main loop.
 */
typedef struct {

    /**
     * A GLib hash table that stores the query string parameters. Do not
     * touch it manually, use the balde_request_get_arg() function instead.
     */
    GHashTable *args;

    /**
     * A GLib hash table that stores the parsed form data. Do not touch it
     * manually, use the balde_request_get_form() function instead.
     */
    GHashTable *form;

    /**
     * Raw request body, if provided by client. You can use it, but not change
     * its value.
     */
    const gchar *stream;

    /**
     * A GLib hash table that stores the arguments parsed from the request
     * path. Do not touch it manually, use the balde_request_get_view_arg()
     * function instead.
     */
    GHashTable *view_args;

    /**
     * A GLib hash table that stores the request headers. Do not touch
     * it manually, use the balde_request_get_header() function instead.
     */
    GHashTable *headers;

    /**
     * A GLib hash table that stores the cookies received from the client.
     * Do not touch it manually, use the balde_request_get_cookie() function
     * instead.
     */
    GHashTable *cookies;

    /**
     * Request path.
     */
    const gchar *path;

    /**
     * Request HTTP method. Can be used to verify which method was used
     * by the client doing the current request.
     */
    balde_http_method_t method;

} balde_request_t;

/** balde response context
 *
 * This struct stores everything related to the response context. It should
 * be initialized by hand for almost every view, using the balde_make_request()
 * function.
 */
typedef struct {

    /**
     * HTTP response status code.
     */
    guint status_code;

    /**
     * A GLib hash table that stores the response headers. Do not touch
     * it manually, use the balde_response_set_header() function instead.
     */
    GHashTable *headers;

    /**
     * A GLib hash table that stores the template variables. Do not touch
     * it manually, use the balde_response_set_tmpl_var() and
     * balde_response_get_tmpl_var() functions instead.
     */
    GHashTable *template_ctx;

    /**
     * A GLib string that stores the response body. Do not touch it
     * manually, use the balde_response_append_body() function instead.
     */
    GString *body;

} balde_response_t;

/** Sets a response header.
 *
 * The header name is case-insensitive.
 */
void balde_response_set_header(balde_response_t *response, const gchar *name,
    const gchar *value);

/** Appends a nul-terminated string to the response body. */
void balde_response_append_body(balde_response_t *response,
    const gchar *content);

/** Appends a string to the response body, with specific length. */
void balde_response_append_body_len(balde_response_t *response,
    const gchar *content, const gssize len);

/** Initialize a response context.
 *
 * The nul-terminated string passed as argument is used to initialize the
 * response body. Please use an empty string if the response doesn't have body,
 * instead of NULL.
 */
balde_response_t* balde_make_response(const gchar *content);

/** Initialize a response context.
 *
 * The string passed as argument is used to initialize the response body.
 * The string size must be passed as the second argument. The string need not
 * to be nul-terminated.
 */
balde_response_t* balde_make_response_len(const gchar *content, const gssize len);

/** Sets a template variable.
 *
 * The template variable name *IS* case-sensitive.
 */
void balde_response_set_tmpl_var(balde_response_t *response, const gchar* name,
    const gchar* value);

/** Gets a template variable.
 *
 * The template variable name *IS* case-sensitive.
 */
const gchar* balde_response_get_tmpl_var(balde_response_t *response,
    const gchar* name);

/** Sets a cookie.
 *
 * Integer arguments should be set to -1 to be ignored.
 * String arguments (excluding name and value) should be set to NULL to be ignored.
 *
 * If the 'expires' argument is less than 0 and 'max_age' is provided, it
 * will be evaluated automatically for you.
 */
void balde_response_set_cookie(balde_response_t *response, const gchar *name,
    const gchar *value, const gint max_age, const gint64 expires,
    const gchar *path, const gchar *domain, const gboolean secure);

/** Deletes a cookie from client.
 *
 * The same arguments provided when setting the cookie should be provided
 * when deleted (if required).
 */
void balde_response_delete_cookie(balde_response_t *response, const gchar *name,
    const gchar *path, const gchar *domain);

/** Gets a request header.
 *
 * The header name is case-insensitive.
 */
const gchar* balde_request_get_header(balde_request_t *request,
    const gchar *name);

/** Gets a query string argument.
 *
 * The argument name *IS* case-sensitive.
 */
const gchar* balde_request_get_arg(balde_request_t *request, const gchar *name);

/** Gets a form input value.
 *
 * The form input name *IS* case-sensitive, and the function will always return
 * NULL for HTTP methods that does not accepts request body.
 */
const gchar* balde_request_get_form(balde_request_t *request, const gchar *name);

/** Gets a view argument.
 *
 * The argument name *IS* case-sensitive.
 */
const gchar* balde_request_get_view_arg(balde_request_t *request,
    const gchar *name);

/** Gets a cookie.
 *
 * The argument name *IS* case-sensitive.
 */
const gchar* balde_request_get_cookie(balde_request_t *request, const gchar *name);

#endif /* _BALDE_WRAPPERS_H */
