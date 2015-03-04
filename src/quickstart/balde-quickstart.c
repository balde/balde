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


static gboolean version = FALSE;
static gchar *app_name = NULL;
static gchar *app_version = NULL;

static GOptionEntry entries[] =
{
    {"version", 0, 0, G_OPTION_ARG_NONE, &version,
        "Show balde's version number and exit.", NULL},
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

    GSList *files = balde_quickstart_list_project_files();
    g_printerr("asd: %d\n", balde_quickstart_check_files(files, "/"));
    balde_quickstart_write_project(files, "bola");


    gchar *project_name = balde_quickstart_get_name(app_name);

    g_printerr("%s\n", project_name);

    g_free(project_name);

    g_free(app_name);
    g_free(app_version);

point1:
    return rv;
}
