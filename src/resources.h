/*
 * balde: A microframework for C based on GLib.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_RESOURCES_PRIVATE_H
#define _BALDE_RESOURCES_PRIVATE_H

#include <glib.h>
#include <gio/gio.h>
#include "balde.h"

typedef struct {
    gchar *name;
    GString *content;
    gchar *type;
    gchar *hash_name;
    gchar *hash_content;
} balde_resource_t;

gchar** balde_resources_list_files(GResource *resources, GError **error);
void balde_resource_free(balde_resource_t *resource);
balde_response_t* balde_make_response_from_static_resource(balde_app_t *app,
    balde_request_t *request, const gchar *name);
balde_response_t* balde_resource_view(balde_app_t *app, balde_request_t *request);

#endif /* _BALDE_RESOURCES_PRIVATE_H */
