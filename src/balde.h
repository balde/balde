/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_H
#define _BALDE_H

#include <glib.h>
#include <gio/gio.h>


/**
 * balde public API.
 *
 * This document describes the public API exposed by balde, to be used by
 * applications.
 *
 * \file balde.h
 *
 */


#define BALDE_LOG_DOMAIN "balde"

#define balde_log_critical(...)                                               \
    g_log(BALDE_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, __VA_ARGS__)

#define balde_log_warning(...)                                                \
    g_log(BALDE_LOG_DOMAIN, G_LOG_LEVEL_WARNING, __VA_ARGS__)

#define balde_log_message(...)                                                \
    g_log(BALDE_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE, __VA_ARGS__)

#define balde_log_info(...)                                                   \
    g_log(BALDE_LOG_DOMAIN, G_LOG_LEVEL_INFO, __VA_ARGS__)

#define balde_log_debug(...)                                                  \
    g_log(BALDE_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, __VA_ARGS__)


/**
 * Supported HTTP methods.
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


/**
 * Supported HTTP status codes.
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


/**
 * balde application context
 *
 * This structure that stores everything related to the application context.
 * It should stay loaded in memory during all the pplication life cycle, as it
 * will be reused for all the requests.
 *
 */
typedef struct {

    /**
     * Application error context.
     *
     */
    GError *error;

    /**
     * Private structure. Shouldn't be touched by users.
     *
     */
    struct _balde_app_private_t *priv;

    /**
     * Private field. Shouldn't be touched by users.
     *
     */
    gboolean copy;

} balde_app_t;


/**
 * balde HTTP authorization context.
 *
 * This struct stores everything related to authorization data sent in the
 * HTTP request.
 *
 * Added in balde 0.2.
 *
 */
typedef struct {

    /**
     * User name.
     *
     */
    const gchar* username;

    /**
     * User password.
     *
     */
    const gchar* password;

    // FIXME: Add HTTP digest support

} balde_authorization_t;


/**
 * balde file representation.
 *
 * This struct stores a file sent by the browser through a file upload.
 *
 * Added in balde 0.2
 *
 */
typedef struct {

    /**
     * File name.
     *
     */
    const gchar *name;

    /**
     * File type.
     *
     */
    const gchar *type;

    /**
     * File content.
     *
     */
    GString *content;

} balde_file_t;


/**
 * balde HTTP request context
 *
 * This struct stores everything related to the request context. It stays
 * loaded in memory during all the request life time, and is initialized
 * automatically by the application main loop.
 *
 */
typedef struct {

    /**
     * A structure that stores the authorization data received from the client.
     *
     */
    balde_authorization_t *authorization;

    /**
     * Request server name.
     *
     */
    const gchar *server_name;

    /**
     * Request script name.
     *
     */
    const gchar *script_name;

    /**
     * Request path.
     *
     */
    const gchar *path;

    /**
     * Request HTTP method. Can be used to verify which method was used
     * by the client doing the current request.
     *
     */
    balde_http_method_t method;

    /**
     * Request using encrypted connection.
     *
     */
    gboolean https;

    /**
     * Private structure. Shouldn't be touched by users.
     *
     */
    struct _balde_request_private_t *priv;

} balde_request_t;


/**
 * balde HTTP response context
 *
 * This struct stores everything related to the response context. It should
 * be initialized by hand for almost every view, using the balde_make_request()
 * function.
 *
 */
typedef struct {

    /**
     * HTTP response status code.
     *
     */
    balde_http_exception_code_t status_code;

    /**
     * Private structure. Shouldn't be touched by users.
     *
     */
    struct _balde_response_private_t *priv;

} balde_response_t;


/**
 * View type definition
 *
 * Each view should accept the application context and the request context,
 * and return a response context.
 *
 */
typedef balde_response_t* (*balde_view_func_t) (balde_app_t*, balde_request_t*);

/**
 * "Before request" hook type definition
 *
 * Each hook should accept the application context and the request context.
 *
 */
typedef void (*balde_before_request_func_t) (balde_app_t*, balde_request_t*);

/**
 * Initializes the application context
 *
 * This function allocates memory for the application context.
 *
 */
balde_app_t* balde_app_init(void);


/**
 * Sets a configuration parameter.
 *
 * The parameter name is case-insensitive.
 *
 */
void balde_app_set_config(balde_app_t *app, const gchar *name, const gchar *value);


/**
 * Sets a configuration parameter with the value of an environment variable.
 *
 * The parameter name is case-insensitive, but the parameter env_name is case
 * sensitive.
 *
 * If the parameter silent is false, this function will raise an error if the
 * environment variable isn't found or is NULL.
 *
 */
void balde_app_set_config_from_envvar(balde_app_t *app, const gchar *name,
    const gchar *env_name, gboolean silent);


/**
 * Gets a configuration parameter.
 *
 * The parameter name is case-insensitive.
 *
 */
const gchar* balde_app_get_config(balde_app_t *app, const gchar *name);


/**
 * Sets user data.
 *
 * The memory needed by the user provided data should be allocated previously,
 * and free'd afterwards, unless you provide a destroy function with
 * balde_app_set_user_data_destroy_func(). If you do not provides a destroy
 * function and call this function twice, it will probably results in a memory
 * leak.
 *
 */
void balde_app_set_user_data(balde_app_t *app, gpointer user_data);


/**
 * Gets user data.
 *
 */
gpointer balde_app_get_user_data(balde_app_t *app);


/**
 * Sets user data destroy function.
 *
 * This function sets the destroy function that will be used by balde to free
 * memory allocated for user data when freeing/setting new values for it.
 *
 */
void balde_app_set_user_data_destroy_func(balde_app_t *app,
    GDestroyNotify destroy_func);


/**
 * Free memory allocated for user data.
 *
 * This function depends on a destroy function being provided through
 * balde_app_set_user_data_destroy_func() function.
 *
 */
void balde_app_free_user_data(balde_app_t *app);


/**
 * Free application context memory.
 *
 * This function will clean and free all memory used by the application context.
 *
 */
void balde_app_free(balde_app_t *app);


/**
 * Adds a view to the balde application
 *
 * The endpoint should be unique, and multiple methods can be provided with pipes.
 *
 */
void balde_app_add_url_rule(balde_app_t *app, const gchar *endpoint,
    const gchar *rule, const balde_http_method_t method,
    balde_view_func_t view_func);


/**
 * Adds a "before request" hook to the balde application
 *
 */
void balde_app_add_before_request(balde_app_t *app,
    balde_before_request_func_t hook_func);


/**
 * Helper function to get the URL for a given endpoint.
 *
 * You should pass the exact number of parameters expected by the view's URL
 * rule.
 *
 * The 'external' parameter isn't implemented yet, but is planned on the API.
 * If set to TRUE, the function will return an absolute URL.
 *
 */
gchar* balde_app_url_for(balde_app_t *app, balde_request_t *request,
    const gchar *endpoint, gboolean external, ...);


/**
 * Application main loop.
 *
 * This function does everything needed to run the registered views and dispatch
 * requests.
 *
 */
void balde_app_run(balde_app_t *app, gint argc, gchar **argv);


/**
 * Sets a response header.
 *
 * The header name is case-insensitive.
 *
 */
void balde_response_set_header(balde_response_t *response, const gchar *name,
    const gchar *value);

/**
 * Sets a response ETag header for the current content of the response.
 *
 * This function should be only used when the response content is ready to be
 * sent to the client, as it can't be overriden later.
 */
void balde_response_set_etag_header(balde_response_t *response,
    gboolean weak);

/**
 * Check if response matches a sent etag header and change reponse to be blank
 * and change response code to 304 Not Modified.
 */
void balde_response_etag_matching(balde_request_t *request,
    balde_response_t *response);

/**
 * Appends a nul-terminated string to the response body.
 *
 */
void balde_response_append_body(balde_response_t *response,
    const gchar *content);


/**
 * Appends a string to the response body, with specific length.
 *
 */
void balde_response_append_body_len(balde_response_t *response,
    const gchar *content, const gssize len);


/**
 * Truncate response's body.
 *
 */
void balde_response_truncate_body(balde_response_t *response);


/**
 * Initialize a response context.
 *
 * The nul-terminated string passed as argument is used to initialize the
 * response body. Please use an empty string if the response doesn't have body,
 * instead of NULL.
 *
 */
balde_response_t* balde_make_response(const gchar *content);


/**
 * Initialize a response context with specific length.
 *
 * The string passed as argument is used to initialize the response body.
 * The string size must be passed as the second argument. The string need not
 * to be nul-terminated.
 *
 */
balde_response_t* balde_make_response_len(const gchar *content, const gssize len);


/**
 * Sets a template variable.
 *
 * The template variable name *IS* case-sensitive.
 *
 */
void balde_response_set_tmpl_var(balde_response_t *response, const gchar* name,
    const gchar* value);


/**
 * Gets a template variable.
 *
 * The template variable name *IS* case-sensitive.
 *
 */
const gchar* balde_response_get_tmpl_var(balde_response_t *response,
    const gchar* name);


/**
 * Gets a template variable and returns an empty string, if not found.
 *
 * The template variable name *IS* case-sensitive.
 *
 */
const gchar* balde_response_get_tmpl_var_or_empty(balde_response_t *response,
    const gchar *name);


/**
 * Sets a cookie.
 *
 * Integer arguments should be set to -1 to be ignored.
 * String arguments (excluding name and value) should be set to NULL to be ignored.
 *
 * If the 'expires' argument is less than 0 and 'max_age' is provided, it
 * will be evaluated automatically for you.
 *
 * Changed in 0.2: added HttpOnly support.
 *
 */
void balde_response_set_cookie(balde_response_t *response, const gchar *name,
    const gchar *value, const gint max_age, const gint64 expires,
    const gchar *path, const gchar *domain, const gboolean secure,
    const gboolean http_only);


/**
 * Deletes a cookie from client.
 *
 * The same arguments provided when setting the cookie should be provided
 * when deleted (if required).
 *
 */
void balde_response_delete_cookie(balde_response_t *response, const gchar *name,
    const gchar *path, const gchar *domain);


/**
 * Gets a request header.
 *
 * The header name is case-insensitive.
 *
 */
const gchar* balde_request_get_header(balde_request_t *request,
    const gchar *name);


/**
 * Gets a query string argument.
 *
 * The argument name *IS* case-sensitive.
 *
 */
const gchar* balde_request_get_arg(balde_request_t *request, const gchar *name);


/**
 * Gets a form input value.
 *
 * The form input name *IS* case-sensitive, and the function will always return
 * NULL for HTTP methods that does not accepts request body.
 *
 */
const gchar* balde_request_get_form(balde_request_t *request, const gchar *name);


/**
 * Gets a file uploaded.
 *
 * The form input name name *IS* case-sensitive, and the function will always
 * return NULL for HTTP methods that does not accepts request body.
 *
 */
const balde_file_t* balde_request_get_file(balde_request_t *request, const gchar *name);


/**
 * Gets a view argument.
 *
 * The argument name *IS* case-sensitive.
 *
 */
const gchar* balde_request_get_view_arg(balde_request_t *request,
    const gchar *name);


/**
 * Gets a cookie.
 *
 * The argument name *IS* case-sensitive.
 *
 */
const gchar* balde_request_get_cookie(balde_request_t *request, const gchar *name);


/**
 * Gets the request body.
 *
 * It returns \c NULL if no body was processed, and a \c GString otherwise.
 *
 */
const GString* balde_request_get_body(balde_request_t *request);


/**
 * Saves a file to disk.
 *
 * destdir is the path of the destination directory, that must exists before
 * this function call. if name is NULL, the default name element of the file
 * structure will be used.
 *
 * Returns the path of the file saved on disk or NULL on errors.
 *
 */
gchar* balde_file_save_to_disk(const balde_file_t *file, const gchar *destdir,
    const gchar *name);


/**
 * Sets application error with an HTTP status code
 *
 * It is called "error" to be consistent with GLib naming standards,
 * but it works like an exception.
 *
 */
void balde_abort_set_error(balde_app_t *app,
    const balde_http_exception_code_t code);


/**
 * Sets application error with an HTTP status code and custom description
 *
 * It is called "error" to be consistent with GLib naming standards,
 * but it works like an exception.
 *
 */
void balde_abort_set_error_with_description(balde_app_t *app,
    const balde_http_exception_code_t code, const gchar* description);


/**
 * Returns a response context that represents an HTTP status code
 *
 * This function returns the response, but does not sets the GLib error.
 *
 */
balde_response_t* balde_abort(balde_app_t *app,
    const balde_http_exception_code_t code);


/**
 * Returns a response context that represents an HTTP status code with custom
 * description
 *
 * This function returns the response, but does not sets the GLib error.
 *
 */
balde_response_t* balde_abort_with_description(balde_app_t *app,
    const balde_http_exception_code_t code, const gchar *description);


/**
 * Initializes an HTTP session context
 *
 */
void balde_session_open(balde_app_t *app, balde_request_t *request);


/**
 * Saves an HTTP session context and attaches it to a response context
 *
 */
void balde_session_save(balde_request_t *request, balde_response_t *response);


/**
 * Gets a value from an HTTP session context
 *
 */
const gchar* balde_session_get(balde_request_t *request, const gchar *key);


/**
 * Sets a value into an HTTP session context
 *
 */
void balde_session_set(balde_request_t *request, const gchar *key, const gchar *value);


/**
 * Deletes a value from an HTTP session context
 *
 */
void balde_session_delete(balde_request_t *request, const gchar *key);


/**
 * Load static resources
 *
 * This function loads all the resources found in a GResource object into
 * the application context.
 */
void balde_resources_load(balde_app_t *app, GResource *resources);


/**
 * Template helper to get the URL for a given endpoint.
 *
 * You should pass the exact number of parameters expected by the view's URL
 * rule.
 *
 * The 'external' parameter isn't implemented yet, but is planned on the API.
 * If set to TRUE, the function will return an absolute URL.
 *
 */
gchar* balde_tmpl_url_for(balde_app_t *app, balde_request_t *request,
    const gchar *endpoint, gboolean external, ...);

#endif /* _BALDE_H */
