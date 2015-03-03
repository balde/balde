/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_QUICKSTART_QUICKSTART_H
#define _BALDE_QUICKSTART_QUICKSTART_H

#include <glib.h>

typedef struct {
    gchar *name;
    GBytes *content;
    gboolean executable;
} balde_quickstart_file_t;

gchar* balde_quickstart_get_name(const gchar *name);
GSList* balde_quickstart_list_project_files(void);

#endif /* _BALDE_QUICKSTART_QUICKSTART_H */
