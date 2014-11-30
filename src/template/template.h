/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_TEMPLATE_TEMPLATE_H
#define _BALDE_TEMPLATE_TEMPLATE_H

#include <glib.h>

gchar* balde_template_generate_source(const gchar *template_name,
    const gchar *template_source);
gchar* balde_template_generate_header(const gchar *template_name);
gchar* balde_template_get_name(const gchar *template_basename);
int balde_template_main(int argc, char **argv);

#endif /* _BALDE_TEMPLATE_TEMPLATE_H */
