/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_H
#define _BALDE_H

#include <glib.h>
#include <gio/gio.h>


/** balde public API.
 *
 * This document describes the public API exposed by balde, to be used by
 * applications.
 *
 * \file balde.h
 */


/** Supported HTTP methods.
 *
 * \c BALDE_HTTP_NONE and \c BALDE_HTTP_ANY are special methods, that will
 * set the view to do not accept any request method or to accept any request
 * method, respectively.
 *
 */
typedef enum {
    BALDE_HTTP_NONE    = 0,
    BALDE_HTTP_OPTIONS = 1 << 0,
    BALDE_HTTP_GET     = 1 << 1,
    BALDE_HTTP_HEAD    = 1 << 2,
    BALDE_HTTP_POST    = 1 << 3,
    BALDE_HTTP_PUT     = 1 << 4,
    BALDE_HTTP_PATCH   = 1 << 5,
    BALDE_HTTP_DELETE  = 1 << 6,
    BALDE_HTTP_ANY     = 0xFF,
} balde_http_method_t;


/** Supported HTTP status codes.
 *
 * These are the HTTP status codes that are supported by the \c balde_abort_*
 * functions. To support any other methods, they must be added to the source
 * code.
 *
 */
typedef enum {
    BALDE_HTTP_OK                              = 200,
    BALDE_HTTP_MULTIPLE_CHOICES                = 300,
    BALDE_HTTP_MOVED_PERMANENTLY               = 301,
    BALDE_HTTP_FOUND                           = 302,
    BALDE_HTTP_SEE_OTHER                       = 303,
    BALDE_HTTP_NOT_MODIFIED                    = 304,
    BALDE_HTTP_USE_PROXY                       = 305,
    BALDE_HTTP_TEMPORARY_REDIRECT              = 307,
    BALDE_HTTP_BAD_REQUEST                     = 400,
    BALDE_HTTP_UNAUTHORIZED                    = 401,
    BALDE_HTTP_FORBIDDEN                       = 403,
    BALDE_HTTP_NOT_FOUND                       = 404,
    BALDE_HTTP_METHOD_NOT_ALLOWED              = 405,
    BALDE_HTTP_NOT_ACCEPTABLE                  = 406,
    BALDE_HTTP_REQUEST_TIMEOUT                 = 408,
    BALDE_HTTP_CONFLICT                        = 409,
    BALDE_HTTP_GONE                            = 410,
    BALDE_HTTP_LENGTH_REQUIRED                 = 411,
    BALDE_HTTP_PRECONDITION_FAILED             = 412,
    BALDE_HTTP_REQUEST_ENTITY_TOO_LARGE        = 413,
    BALDE_HTTP_REQUEST_URI_TOO_LONG            = 414,
    BALDE_HTTP_UNSUPPORTED_MEDIA_TYPE          = 415,
    BALDE_HTTP_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
    BALDE_HTTP_EXPECTATION_FAILED              = 417,
    BALDE_HTTP_I_M_A_TEAPOT                    = 418,
    BALDE_HTTP_UNPROCESSABLE_ENTITY            = 422,
    BALDE_HTTP_PRECONDITION_REQUIRED           = 428,
    BALDE_HTTP_TOO_MANY_REQUESTS               = 429,
    BALDE_HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    BALDE_HTTP_INTERNAL_SERVER_ERROR           = 500,
    BALDE_HTTP_NOT_IMPLEMENTED                 = 501,
    BALDE_HTTP_BAD_GATEWAY                     = 502,
    BALDE_HTTP_SERVICE_UNAVAILABLE             = 503,
} balde_http_exception_code_t;


/** balde application context
 *
 * This is an opaque structure that stores everything related to the
 * application context. It should stay loaded in memory during all the
 * application life cycle, as it will be reused for all the requests.
 */
typedef struct {

    /** Application error context. */
    GError *error;

    /** Private structure. Shouldn't be touched by users. **/
    struct _balde_app_private_t *priv;

} balde_app_t;

/**
 * \example hello-with-auth.c
 *
 * An example with Basic HTTP authentication.
 */

/** balde HTTP authorization context.
 *
 * This struct stores everything related to authorization data sent by the
 * HTTP request.
 *
 * Added in balde 0.2.
 */
typedef struct {

    /**
     * User name.
     */
    const gchar* username;

    /**
     * User password.
     */
    const gchar* password;

    // FIXME: Add HTTP digest support

} balde_authorization_t;

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
     * A structure that stores the authorization data received from the client.
     */
    balde_authorization_t *authorization;

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
 *
 * Changed in 0.2: added HttpOnly support.
 */
void balde_response_set_cookie(balde_response_t *response, const gchar *name,
    const gchar *value, const gint max_age, const gint64 expires,
    const gchar *path, const gchar *domain, const gboolean secure,
    const gboolean http_only);

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



/** Load static resources
 *
 * This function loads all the resources found in a GResource object into
 * the application context.
 */
void balde_resources_load(balde_app_t *app, GResource *resources);



/** Template helper to get the URL for a given endpoint.
 *
 * You should pass the exact number of parameters expected by the view's URL
 * rule.
 *
 * The 'external' parameter isn't implemented yet, but is planned on the API.
 * If set to TRUE, the function will return an absolute URL.
 */
gchar* balde_tmpl_url_for(balde_app_t *app, balde_request_t *request,
    const gchar *endpoint, gboolean external, ...);

#endif /* _BALDE_H */
