/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
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
#include "template.h"
#include "config.h"
#include "static.h"
#include "theme.h"

static gboolean version = FALSE;
static gboolean dependencies = FALSE;
static gchar *sourcedir = NULL;
static gchar *target = NULL;
static GOptionEntry entries[] =
{
    {"version", 0, 0, G_OPTION_ARG_NONE, &version,
        "Show balde's version number and exit.", NULL},
    {"dependencies", 0, 0, G_OPTION_ARG_NONE, &dependencies,
        "Generate dependency list, suitable for Makefile rules.", NULL},
    {"sourcedir", 0, 0, G_OPTION_ARG_STRING, &sourcedir,
        "The directory where files are to be read from (default to current directory)",
        "DIRECTORY"},
    {"target", 0, 0, G_OPTION_ARG_STRING, &target,
        "Name of the output file.", "FILE"},
    {NULL}
};


int
main(int argc, char **argv)
{
    setlocale(LC_ALL, "");
    int rv = EXIT_SUCCESS;
    GError *err = NULL;
    GOptionContext *context = g_option_context_new(
        "JSON_FILE - balde theme source code generator");
    g_option_context_add_main_entries(context, entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &err)) {
        g_printerr("Option parsing failed: %s\n", err->message);
        g_clear_error(&err);
        rv = EXIT_FAILURE;
        goto point1;
    }
    if (version) {
        g_printerr("%s\n", PACKAGE_STRING);
        goto point1;
    }
    if (argc != 2) {
        g_printerr("Error: You should give exactly one JSON file name.\n");
        rv = EXIT_FAILURE;
        goto point1;
    }
    if (dependencies && (target != NULL)) {
        g_printerr("Error: --dependencies and --target are mutually exclusive.\n");
        rv = EXIT_FAILURE;
        goto point1;
    }

    balde_theme_config_t *config = balde_theme_config_read(argv[1]);
    if (config == NULL)
        goto point1;

    if (sourcedir == NULL)
        sourcedir = g_strdup("");

    if (dependencies) {
        for (guint i = 0; config->templates[i] != NULL; i++) {
            gchar *f = g_build_filename(sourcedir, config->templates[i], NULL);
            g_print("%s\n", f);
            g_free(f);
        }
        for (guint i = 0; config->static_resources[i] != NULL; i++) {
            gchar *f = g_build_filename(sourcedir, config->static_resources[i], NULL);
            g_print("%s\n", f);
            g_free(f);
        }
        goto point1;
    }

    GBytes *s = balde_theme_static_get_resource_data(sourcedir, config->static_resources);
    balde_theme_generate_source(sourcedir, config);

    g_printerr("bola: %s\n", balde_theme_static_render_resource_data(s));


    /*gchar *source = NULL;
    gchar *template_name = balde_template_get_name(argv[2]);
    if (g_str_has_suffix(argv[2], ".c")) {
        source = balde_template_generate_source(template_name, argv[1]);
    }
    else if (g_str_has_suffix(argv[2], ".h")) {
        source = balde_template_generate_header(template_name);
    }
    else {
        g_printerr("Invalid filename: %s\n", argv[2]);
        rv = EXIT_FAILURE;
        goto point2;
    }
    if (!g_file_set_contents(argv[2], source, -1, NULL)) {
        g_printerr("Failed to write file: %s\n", argv[2]);
        rv = EXIT_FAILURE;
        goto point2;  // duh!
    }*/

//point2:
    //g_free(source);
    //g_free(template_name);
point1:
    g_free(target);
    g_option_context_free(context);
    return rv;
}
