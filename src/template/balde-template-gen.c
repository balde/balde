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

static gboolean version = FALSE;
static gboolean generate_dependencies = FALSE;
static GOptionEntry entries[] =
{
    {"version", 0, 0, G_OPTION_ARG_NONE, &version,
        "Show balde's version number and exit.", NULL},
    {"generate-dependencies", 0, 0, G_OPTION_ARG_NONE, &generate_dependencies,
        "Generate dependency list, suitable for Makefile rules.", NULL},
    {NULL}
};


int
main(int argc, char **argv)
{
    setlocale(LC_ALL, "");
    int rv = EXIT_SUCCESS;
    GError *err = NULL;
    GOptionContext *context = g_option_context_new(
        "TEMPLATE [GENERATED-FILE] - balde template source code generator");
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
    if (argc >= 2 && generate_dependencies) {
        gchar *dependencies = balde_template_generate_dependencies(argv[1]);
        g_print("%s\n", dependencies);
        g_free(dependencies);
        goto point1;
    }
    if (argc != 3) {
        gchar *help = g_option_context_get_help(context, FALSE, NULL);
        g_printerr("%s", help);
        g_free(help);
        rv = EXIT_FAILURE;
        goto point1;
    }
    gchar *source = NULL;
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
    }

point2:
    g_free(source);
    g_free(template_name);
point1:
    g_option_context_free(context);
    return rv;
}
