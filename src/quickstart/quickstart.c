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
#include <stdlib.h>
#include "quickstart.h"


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
