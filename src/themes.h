/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_THEMES_PRIVATE_H
#define _BALDE_THEMES_PRIVATE_H

#include <glib.h>
#include "balde.h"

struct _balde_theme_private_t {
    GBytes *static_resources;
    GHashTable *templates;
};

balde_theme_t* balde_theme_new(guint8 *static_data, gsize static_data_len);
void balde_theme_add_template(balde_theme_t *theme, const gchar *name,
    balde_template_func_t func);
void balde_theme_free(balde_theme_t *theme);

#endif /* _BALDE_THEMES_PRIVATE_H */
