/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013 Rafael G. Martins <rafael@rafaelmartins.eng.br>
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
    BALDE_HTTP_NONE = 0x00,
    BALDE_HTTP_OPTIONS = 0x01,
    BALDE_HTTP_GET = 0x02,
    BALDE_HTTP_HEAD = 0x04,
    BALDE_HTTP_POST = 0x08,
    BALDE_HTTP_PUT = 0x10,
    BALDE_HTTP_DELETE = 0x20
} balde_http_method_t;

#endif /* _BALDE_ROUTING_H */
