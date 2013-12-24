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

typedef struct {
    GHashTable *view_args;
    GHashTable *headers;
    gchar *path;
    balde_http_method_t method;
} balde_request_t;

typedef struct {
    guint status_code;
    GHashTable *headers;
    GString *body;
} balde_response_t;

void balde_response_set_header(balde_response_t *response, gchar *name, gchar *value);
void balde_response_append_body(balde_response_t *response, gchar *content);
balde_response_t* balde_make_response(gchar *content);
gchar* balde_request_get_header(balde_request_t *request, gchar *name);

#endif /* _BALDE_WRAPPERS_H */
