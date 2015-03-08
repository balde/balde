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
static gchar *app_name = NULL;
static gchar *app_version = NULL;
static gboolean force = FALSE;

static GOptionEntry entries[] =
{
    {"version", 0, 0, G_OPTION_ARG_NONE, &version,
        "Show balde's version number and exit.", NULL},
    {"app-name", 0, 0, G_OPTION_ARG_STRING, &app_name,
        "Application name. (default: application directory basename)", "NAME"},
    {"app-version", 0, 0, G_OPTION_ARG_STRING, &app_version,
        "Application version. (default: 0)", "VERSION"},
    {"force", 0, 0, G_OPTION_ARG_NONE, &force,
        "Force overwriting of existing files in application directory.", NULL},
    {NULL}
};


int
main(int argc, char **argv)
{
    setlocale(LC_ALL, "");
    int rv = EXIT_SUCCESS;
    GError *err = NULL;
    GOptionContext *context = g_option_context_new(
        "APP_DIR - a helper tool to bootstrap your balde application");
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
    if (argc != 2) {
        gchar *help = g_option_context_get_help(context, FALSE, NULL);
        g_printerr("%s", help);
        g_free(help);
        rv = EXIT_FAILURE;
        goto point1;
    }

    // get project files from binary
    GSList *files = balde_quickstart_list_project_files(project_get_resource());

    gchar *app_dir = argv[1];

    if (!force) {

        gchar **collisions = balde_quickstart_check_files(files, app_dir);

        if (collisions != NULL) {
            g_printerr(
                "error: some files in the output directory would be overwritten:\n\n");
            gchar *tmp_collisions = g_strjoinv("\n\t", collisions);
            g_printerr(
                "\t%s\n\n", tmp_collisions);
            g_free(tmp_collisions);
            g_printerr(
                "if you really want to overwrite these files, run this program "
                "again adding --force option.\n");
            g_strfreev(collisions);
            goto point2;
        }
    }

    if (app_name == NULL)
        app_name = balde_quickstart_get_app_name(app_dir);

    if (app_version == NULL)
        app_version = g_strdup("0");

    balde_quickstart_write_project(files, app_dir, app_name, app_version);

point2:
    g_free(app_name);
    g_free(app_version);

    balde_quickstart_free_files(files);

point1:
    g_option_context_free(context);

    return rv;
}
