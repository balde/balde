/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_THEME_THEME_H
#define _BALDE_THEME_THEME_H

#include <glib.h>
#include "config.h"

GString* balde_theme_generate_source(const gchar *sourcedir, balde_theme_config_t *config);

#endif /* _BALDE_THEME_THEME_H */
