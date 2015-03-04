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

#include <locale.h>
#include <stdlib.h>
#include "quickstart.h"
#include "project.h"


static gboolean version = FALSE;
static gchar *app_dir = NULL;
static gchar *app_name = NULL;
static gchar *app_version = NULL;

static GOptionEntry entries[] =
{
    {"version", 0, 0, G_OPTION_ARG_NONE, &version,
        "Show balde's version number and exit.", NULL},
    {"app-dir", 0, 0, G_OPTION_ARG_FILENAME, &app_dir,
        "Application directory, where files will be created. (default: current "
        "directory)", "DIR"},
    {"app-name", 0, 0, G_OPTION_ARG_STRING, &app_name,
        "Application name. (default: current directory name)", "NAME"},
    {"app-version", 0, 0, G_OPTION_ARG_STRING, &app_version,
        "Application version. (default: 0.1)", "VERSION"},
    {NULL}
};


int
main(int argc, char **argv)
{
    setlocale(LC_ALL, "");
    int rv = EXIT_SUCCESS;
    GError *err = NULL;
    GOptionContext *context = g_option_context_new(
        "- a helper tool to bootstrap your balde application");
    g_option_context_add_main_entries(context, entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &err)) {
        g_printerr("Option parsing failed: %s\n", err->message);
        rv = EXIT_FAILURE;
        goto point1;
    }
    if (version) {
        g_printerr("%s\n", PACKAGE_STRING);
        goto point1;
    }

    // get project files from binary
    GSList *files = balde_quickstart_list_project_files(project_get_resource());

    if (app_dir == NULL)
        app_dir = g_get_current_dir();

    if (!balde_quickstart_check_files(files, app_dir)) {
        g_printerr(
            "error: some files in the output directory (%s) would be overwritten.\n",
            app_dir);
        g_printerr(
            "       if you really want to use this directory, clean it up and "
            "run this program again.\n");
        goto point2;
    }

    if (app_name == NULL)
        app_name = balde_quickstart_get_name();

    if (app_version == NULL)
        app_version = g_strdup("0.1");

    balde_quickstart_write_project(files, app_dir, app_name, app_version);

point2:
    g_free(app_dir);
    g_free(app_name);
    g_free(app_version);

    balde_quickstart_free_files(files);

point1:
    g_option_context_free(context);

    return rv;
}
