/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <gio/gio.h>
#include "quickstart.h"
#include "project.h"


gchar*
balde_quickstart_get_name(const gchar *name)
{
    if (name != NULL)
        return g_strdup(name);

    gchar *cwd = g_get_current_dir();
    gchar *dirname = g_path_get_basename(cwd);
    g_free(cwd);
    if (g_strcmp0(dirname, ".") == 0 || g_strcmp0(dirname, "/") == 0) {
        g_free(dirname);
        return g_strdup("unnamed");
    }
    return dirname;
}


GSList*
balde_quickstart_list_project_files(void)
{
    GResource *resource = project_get_resource();
    gchar **resources = g_resource_enumerate_children(resource, "/", 0, NULL);
    GSList *files = NULL;
    for (guint i = 0; resources[i] != NULL; i++) {
        g_printerr("%s\n", resources[i]);
    }
    g_strfreev(resources);
    return files;
}
