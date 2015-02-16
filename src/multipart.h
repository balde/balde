/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_MULTIPART_PRIVATE_H
#define _BALDE_MULTIPART_PRIVATE_H

#include <glib.h>

typedef struct {
    GHashTable *files;
    GHashTable *form;
} balde_multipart_data_t;

typedef struct {
    gchar *header_name;
    gchar *file_name;
    gchar *file_type;
    GString *file_content;
    gchar *field_name;
    balde_multipart_data_t *data;
} balde_multipart_state_t;

gchar* balde_multipart_parse_boundary(const gchar *header);
balde_multipart_data_t* balde_multipart_parse(const gchar *boundary, const GString *body);

#endif /* _BALDE_MULTIPART_PRIVATE_H */
