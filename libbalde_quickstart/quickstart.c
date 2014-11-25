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
#include <locale.h>
#include <stdlib.h>
#include <balde-quickstart/quickstart-private.h>


static gchar*
get_name(const gchar *name)
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


static gboolean version = FALSE;
static gchar *name = NULL;

static GOptionEntry entries[] =
{
    {"version", 0, 0, G_OPTION_ARG_NONE, &version,
        "Show balde's version number and exit.", NULL},
    {"name", 'n', 0, G_OPTION_ARG_STRING, &name,
        "Application name. (default: current directory name)", "NAME"},
    {NULL}
};


int
balde_quickstart_main(int argc, char **argv)
{
    setlocale(LC_ALL, "");
    GError *err = NULL;
    GOptionContext *context = g_option_context_new(
        "- a helper tool to bootstrap your balde application");
    g_option_context_add_main_entries(context, entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &err)) {
        g_printerr("Option parsing failed: %s\n", err->message);
        exit(1);
    }
    if (version)
        g_printerr("%s\n", PACKAGE_STRING);

    gchar *project_name = get_name(name);

    g_printerr("%s\n", project_name);


    g_free(project_name);
    return 0;
}
