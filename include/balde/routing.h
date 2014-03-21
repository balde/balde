/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_ROUTING_H
#define _BALDE_ROUTING_H

#include <glib.h>

/** URL routing related definitions.
 *
 * \file balde/routing.h
 */

/** Supported HTTP methods.
 *
 * BALDE_HTTP_NONE method is internal and shouldn't be used manually.
 */
typedef enum {
    BALDE_HTTP_NONE    = 0,
    BALDE_HTTP_OPTIONS = 1 << 0,
    BALDE_HTTP_GET     = 1 << 1,
    BALDE_HTTP_HEAD    = 1 << 2,
    BALDE_HTTP_POST    = 1 << 3,
    BALDE_HTTP_PUT     = 1 << 4,
    BALDE_HTTP_PATCH   = 1 << 5,
    BALDE_HTTP_DELETE  = 1 << 6
} balde_http_method_t;

#endif /* _BALDE_ROUTING_H */
