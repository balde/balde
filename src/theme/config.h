/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_THEME_CONFIG_H
#define _BALDE_THEME_CONFIG_H

#include <glib.h>

typedef struct {
    gchar **templates;
    gchar **static_resources;
    gchar **includes;
} balde_theme_config_t;

balde_theme_config_t* balde_theme_config_read(const gchar *filepath);

#endif /* _BALDE_THEME_CONFIG_H */
