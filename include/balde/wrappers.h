/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013 Rafael G. Martins <rafael@rafaelmartins.eng.br>
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
     * A GLib hash table that stores the arguments parsed from the request
     * path.
     */
    GHashTable *view_args;

    /**
     * A GLib hash table that stores the request headers. Do not touch
     * it manually, use the balde_request_get_header() function instead.
     */
    GHashTable *headers;

    /**
     * Request path.
     */
    gchar *path;

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
     * A GLib string that stores the response body. Do not touch it
     * manually, use the balde_response_append_body() function instead.
     */
    GString *body;

} balde_response_t;

/** Sets a response header.
 *
 * The header name is case-insensitive.
 */
void balde_response_set_header(balde_response_t *response, gchar *name, gchar *value);

/** Appends a string to the response body. */
void balde_response_append_body(balde_response_t *response, gchar *content);

/** Initialize a response context.
 *
 * The string passed as argument is used to initialize the response body.
 * Please use an empty string if the response doesn't have body, instead
 * of NULL.
 */
balde_response_t* balde_make_response(gchar *content);

/** Gets a request header.
 *
 * The header name is case insensitive
 */
gchar* balde_request_get_header(balde_request_t *request, gchar *name);

#endif /* _BALDE_WRAPPERS_H */
