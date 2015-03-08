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
#include <gio/gio.h>

typedef struct {
    gchar *name;
    GString *content;
    gboolean executable;
} balde_quickstart_file_t;

gchar* balde_quickstart_get_app_name(const gchar *dir);
gchar* balde_quickstart_fix_app_name(const gchar *app_name, gchar replace);
GSList* balde_quickstart_list_project_files(GResource *resource);
void balde_quickstart_free_files(GSList *l);
gchar** balde_quickstart_check_files(GSList *files, const gchar *dir);
void balde_quickstart_write_project(GSList *files, const gchar *dir,
    const gchar *app_name, const gchar *app_version);

#endif /* _BALDE_QUICKSTART_QUICKSTART_H */
