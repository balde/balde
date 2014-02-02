/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_EXCEPTIONS_H
#define _BALDE_EXCEPTIONS_H

#include <glib.h>
#include <balde/app.h>
#include <balde/wrappers.h>

/** HTTP exceptions related definitions.
 *
 * balde handles HTTP status codes as if they were GLib exceptions (or
 * "errors", following GLib naming standards).
 *
 * \file balde/exceptions.h
 */

/** Supported HTTP status codes */
typedef enum {
    BALDE_HTTP_BAD_REQUEST = 400,
    BALDE_HTTP_UNAUTHORIZED = 401,
    BALDE_HTTP_FORBIDDEN = 403,
    BALDE_HTTP_NOT_FOUND = 404,
    BALDE_HTTP_METHOD_NOT_ALLOWED = 405,
    BALDE_HTTP_NOT_ACCEPTABLE = 406,
    BALDE_HTTP_REQUEST_TIMEOUT = 408,
    BALDE_HTTP_CONFLICT = 409,
    BALDE_HTTP_GONE = 410,
    BALDE_HTTP_LENGTH_REQUIRED = 411,
    BALDE_HTTP_PRECONDITION_FAILED = 412,
    BALDE_HTTP_REQUEST_ENTITY_TOO_LARGE = 413,
    BALDE_HTTP_REQUEST_URI_TOO_LONG = 414,
    BALDE_HTTP_UNSUPPORTED_MEDIA_TYPE = 415,
    BALDE_HTTP_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
    BALDE_HTTP_EXPECTATION_FAILED = 417,
    BALDE_HTTP_I_M_A_TEAPOT = 418,
    BALDE_HTTP_UNPROCESSABLE_ENTITY = 422,
    BALDE_HTTP_PRECONDITION_REQUIRED = 428,
    BALDE_HTTP_TOO_MANY_REQUESTS = 429,
    BALDE_HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    BALDE_HTTP_INTERNAL_SERVER_ERROR = 500,
    BALDE_HTTP_NOT_IMPLEMENTED = 501,
    BALDE_HTTP_BAD_GATEWAY = 502,
    BALDE_HTTP_SERVICE_UNAVAILABLE = 503
} balde_http_exception_code_t;

/** Sets application error with an HTTP status code
 *
 * It is called "error" to be consistent with GLib naming standards,
 * but it works like an exception.
 */
void balde_abort_set_error(balde_app_t *app,
    const balde_http_exception_code_t code);

/** Sets application error with an HTTP status code and custom description
 *
 * It is called "error" to be consistent with GLib naming standards,
 * but it works like an exception.
 */
void balde_abort_set_error_with_description(balde_app_t *app,
    const balde_http_exception_code_t code, const gchar* description);

/** Returns a response context that represents an HTTP status code
 *
 * This function returns the response, but does not sets the GLib error.
 */
balde_response_t* balde_abort(balde_app_t *app,
    const balde_http_exception_code_t code);

/** Returns a response context that represents an HTTP status code with custom
 * description
 *
 * This function returns the response, but does not sets the GLib error.
 */
balde_response_t* balde_abort_with_description(balde_app_t *app,
    const balde_http_exception_code_t code, const gchar *description);

#endif /* _BALDE_EXCEPTIONS_H */
