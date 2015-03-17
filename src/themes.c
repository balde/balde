/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include "balde.h"
#include "themes.h"


balde_theme_t*
balde_theme_new(guint8 *static_data, gsize static_data_len)
{
    balde_theme_t *theme = g_new(balde_theme_t, 1);
    theme->static_resources = g_bytes_new(static_data, static_data_len);
    theme->templates = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    return theme;
}


void
balde_theme_add_template(balde_theme_t *theme, const gchar *name,
    balde_template_func_t func)
{
    g_hash_table_replace(theme->templates, g_strdup(name), func);
}


void
balde_theme_free(balde_theme_t *theme)
{
    if (theme == NULL)
        return;
    g_bytes_unref(theme->static_resources);
    g_hash_table_destroy(theme->templates);
    g_free(theme);
}
