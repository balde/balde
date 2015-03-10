/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_TEMPLATE_TEMPLATE_H
#define _BALDE_TEMPLATE_TEMPLATE_H

#include <glib.h>

typedef struct {
    GSList *includes;
    GSList *imports;
    GString *body;
    guint indent;
    gboolean declare_tmp;
} balde_template_state_t;

void balde_template_build_state(const gchar *filename, balde_template_state_t **state);
void balde_template_free_state(balde_template_state_t *state);
gchar* balde_template_generate_source(const gchar *template_name, const gchar *file_name);
gchar* balde_template_generate_header(const gchar *template_name);
gchar* balde_template_generate_dependencies(const gchar *file_name);
gchar* balde_template_get_name(const gchar *template_basename);

#endif /* _BALDE_TEMPLATE_TEMPLATE_H */
