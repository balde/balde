/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_THEME_STATIC_H
#define _BALDE_THEME_STATIC_H

#include <glib.h>

gchar* balde_theme_static_generate_resource_xml(gchar **files);
GBytes* balde_theme_static_get_resource_data(const gchar *sourcedir, gchar **files);
gchar* balde_theme_static_render_resource_data(GBytes *d);

#endif /* _BALDE_THEME_STATIC_H */
