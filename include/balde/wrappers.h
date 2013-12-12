/**
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_WRAPPERS_H
#define _BALDE_WRAPPERS_H

#include <glib.h>

typedef struct {
    GHashTable *headers;
    gchar* path;
} balde_request_t;

typedef struct {
    guint status_code;
    GHashTable *headers;
    GString *body;
} balde_response_t;

#endif /* _BALDE_WRAPPERS_H */
