/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <balde/app.h>
#include <balde/app-private.h>
#include <balde/exceptions.h>
#include <balde/exceptions-private.h>
#include <balde/wrappers.h>
#include <balde/wrappers-private.h>


const static balde_http_exception_t exceptions[] = {
    {
        .code = BALDE_HTTP_BAD_REQUEST,  // 400
        .name = "Bad Request",
        .description =
            "The browser (or proxy) sent a request that this server could "
            "not understand."
    },
    {
        .code = BALDE_HTTP_UNAUTHORIZED,  // 401
        .name = "Unauthorized",
        .description =
            "The server could not verify that you are authorized to access the "
            "URL requested. You either supplied the wrong credentials (e.g. a "
            "bad password), or your browser doesn't understand how to supply "
            "the credentials required."
    },
    {
        .code = BALDE_HTTP_FORBIDDEN,  // 403
        .name = "Forbidden",
        .description =
            "You don't have the permission to access the requested resource. It "
            "is either read-protected or not readable by the server."
    },
    {
        .code = BALDE_HTTP_NOT_FOUND,  // 404
        .name = "Not Found",
        .description =
            "The requested URL was not found on the server. If you entered the "
            "URL manually please check your spelling and try again."
    },
    {
        .code = BALDE_HTTP_METHOD_NOT_ALLOWED,  // 405
        .name = "Method Not Allowed",
        .description =
            "The method is not allowed for the requested URL."
    },
    {
        .code = BALDE_HTTP_NOT_ACCEPTABLE,  // 406
        .name = "Not Acceptable",
        .description =
            "The resource identified by the request is only capable of generating "
            "response entities which have content characteristics not acceptable "
            "according to the accept headers sent in the request."
    },
    {
        .code = BALDE_HTTP_REQUEST_TIMEOUT,  // 408
        .name = "Request Timeout",
        .description =
            "The server closed the network connection because the browser didn't "
            "finish the request within the specified time."
    },
    {
        .code = BALDE_HTTP_CONFLICT,  // 409
        .name = "Conflict",
        .description =
            "A conflict happened while processing the request. The resource might "
            "have been modified while the request was being processed."
    },
    {
        .code = BALDE_HTTP_GONE,  // 410
        .name = "Gone",
        .description =
            "The requested URL is no longer available on this server and there is "
            "no forwarding address. If you followed a link from a foreign page, "
            "please contact the author of this page."
    },
    {
        .code = BALDE_HTTP_LENGTH_REQUIRED,  // 411
        .name = "Length Required",
        .description =
            "A request with this method requires a valid Content-Length header."
    },
    {
        .code = BALDE_HTTP_PRECONDITION_FAILED,  // 412
        .name = "Precondition Failed",
        .description =
            "The precondition on the request for the URL failed positive evaluation."
    },
    {
        .code = BALDE_HTTP_REQUEST_ENTITY_TOO_LARGE,  // 413
        .name = "Request Entity Too Large",
        .description =
            "The data value transmitted exceeds the capacity limit."
    },
    {
        .code = BALDE_HTTP_REQUEST_URI_TOO_LONG,  // 414
        .name = "Request URI Too Long",
        .description =
            "The length of the requested URL exceeds the capacity limit for this "
            "server. The request cannot be processed."
    },
    {
        .code = BALDE_HTTP_UNSUPPORTED_MEDIA_TYPE,  // 415
        .name = "Unsupported Media Type",
        .description =
            "The server does not support the media type transmitted in the request."
    },
    {
        .code = BALDE_HTTP_REQUESTED_RANGE_NOT_SATISFIABLE,  // 416
        .name = "Requested Range Not Satisfiable",
        .description =
            "The server cannot provide the requested range."
    },
    {
        .code = BALDE_HTTP_EXPECTATION_FAILED,  // 417
        .name = "Expectation Failed",
        .description =
            "The server could not meet the requirements of the Expect header"
    },
    {
        .code = BALDE_HTTP_I_M_A_TEAPOT,  // 418
        .name = "I'm a teapot",
        .description = "This server is a teapot, not a coffee machine"
    },
    {
        .code = BALDE_HTTP_UNPROCESSABLE_ENTITY,  // 422
        .name = "Unprocessable Entity",
        .description =
            "The request was well-formed but was unable to be followed due to "
            "semantic errors."
    },
    {
        .code = BALDE_HTTP_PRECONDITION_REQUIRED,  // 428
        .name = "Precondition Required",
        .description =
            "This request is required to be conditional; try using \"If-Match\" "
            "or \"If-Unmodified-Since\"."
    },
    {
        .code = BALDE_HTTP_TOO_MANY_REQUESTS,  // 429
        .name = "Too Many Requests",
        .description =
            "This user has exceeded an allotted request count. Try again later."
    },
    {
        .code = BALDE_HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE,  // 431
        .name = "Request Header Fields Too Large",
        .description =
            "One or more header fields exceeds the maximum size."
    },
    {
        .code = BALDE_HTTP_INTERNAL_SERVER_ERROR,  // 500
        .name = "Internal Server Error",
        .description =
            "The server encountered an internal error and was unable to complete "
            "your request. Either the server is overloaded or there is an error "
            "in the application."
    },
    {
        .code = BALDE_HTTP_NOT_IMPLEMENTED,  // 501
        .name = "Not Implemented",
        .description =
            "The server does not support the action requested by the browser."
    },
    {
        .code = BALDE_HTTP_BAD_GATEWAY,  // 502
        .name = "Bad Gateway",
        .description =
            "The proxy server received an invalid response from an upstream server."
    },
    {
        .code = BALDE_HTTP_SERVICE_UNAVAILABLE,  // 503
        .name = "Service Unavailable",
        .description =
            "The server is temporarily unable to service your request due to "
            "maintenance downtime or capacity problems. Please try again later."
    },
    {0, NULL, NULL}
};


GQuark
balde_http_exception_quark(void)
{
    return g_quark_from_static_string("balde-http-exception-quark");
}


const gchar*
balde_exception_get_name_from_code(const balde_http_exception_code_t code)
{
    for (guint i = 0; exceptions[i].name != NULL; i++)
        if (exceptions[i].code == code)
            return exceptions[i].name;
    return NULL;
}


const gchar*
balde_exception_get_description_from_code(const balde_http_exception_code_t code)
{
    for (guint i = 0; exceptions[i].name != NULL; i++)
        if (exceptions[i].code == code)
            return exceptions[i].description;
    return NULL;
}


void
balde_abort_set_error(balde_app_t *app, const balde_http_exception_code_t code)
{
    g_propagate_error(&(app->error),
        g_error_new(balde_http_exception_quark(), code,
            "%s", balde_exception_get_description_from_code(code)));
}


void
balde_abort_set_error_with_description(balde_app_t *app,
    const balde_http_exception_code_t code, const gchar* description)
{
    g_propagate_error(&(app->error),
        g_error_new(balde_http_exception_quark(), code,
            "%s\n\n%s", balde_exception_get_description_from_code(code),
            description));
}


balde_response_t*
balde_abort(balde_app_t *app, const balde_http_exception_code_t code)
{
    balde_abort_set_error(app, code);
    balde_response_t* response = NULL;
    if (app->error != NULL) {
        response = balde_make_response_from_exception(app->error);
        g_clear_error(&app->error);
        app->error = NULL;
    }
    return response;
}


balde_response_t*
balde_abort_with_description(balde_app_t *app,
    const balde_http_exception_code_t code, const gchar *description)
{
    balde_abort_set_error_with_description(app, code, description);
    balde_response_t* response = NULL;
    if (app->error != NULL) {
        response = balde_make_response_from_exception(app->error);
        g_clear_error(&app->error);
        app->error = NULL;
    }
    return response;
}
