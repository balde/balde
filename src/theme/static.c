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
#include <glib/gstdio.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */


gchar*
balde_static_generate_resource_xml(gchar **files)
{
    GString *rv = g_string_new(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<gresources>\n"
        "    <gresource prefix=\"/static\">\n");

    for (guint i = 0; files[i] != NULL; i++)
        g_string_append_printf(rv,
            "        <file>%s</file>\n", files[i]);

    g_string_append(rv,
        "    </gresource>\n"
        "</gresources>\n");
    return g_string_free(rv, FALSE);
}


GBytes*
balde_static_get_resource_data(const gchar *sourcedir, gchar **files)
{
    GBytes *rv = NULL;
    gchar *gcr = g_find_program_in_path(GLIB_COMPILE_RESOURCES);
    if (gcr == NULL) {
        g_printerr("Failed to find " GLIB_COMPILE_RESOURCES " binary\n");
        goto point1;
    }

    gchar *tmp_file = g_strdup("balde_resource-XXXXXXXX.gresource");
    gchar *tmp_file2 = g_strdup("balde_resource-XXXXXXXX.xml");
    gint fd;

    if ((fd = g_mkstemp(tmp_file)) == -1) {
        g_printerr("Failed to create temporary file: %s\n", tmp_file);
        goto point2;
    }
    close(fd);

    if ((fd = g_mkstemp(tmp_file2)) == -1) {
        g_printerr("Failed to create temporary file: %s\n", tmp_file2);
        goto point2;
    }
    close(fd);

    gchar *xml = balde_static_generate_resource_xml(files);
    if (!g_file_set_contents(tmp_file2, xml, -1, NULL)) {
        g_printerr("Failed to write to temporary file: %s\n", tmp_file2);
        g_free(xml);
        goto point2;
    }
    g_free(xml);

    gchar *argv[7];
    gint argc = 0;
    argv[argc++] = gcr;
    argv[argc++] = "--target";
    argv[argc++] = tmp_file;
    argv[argc++] = "--sourcedir";
    argv[argc++] = (gchar*) sourcedir;
    argv[argc++] = tmp_file2;
    argv[argc++] = NULL;
    g_assert(argc <= G_N_ELEMENTS(argv));

    gchar *stderr_child = NULL;
    gint status;
    if (!g_spawn_sync(NULL, argv, NULL, G_SPAWN_STDOUT_TO_DEV_NULL,
            NULL, NULL, NULL, &stderr_child, &status, NULL)) {
        g_printerr("Error running " GLIB_COMPILE_RESOURCES " binary:\n%s\n",
            stderr_child);
        goto point3;
    }
    if (status != 0) {
        g_printerr("Error running " GLIB_COMPILE_RESOURCES " binary (%d):\n%s\n",
            status, stderr_child);
        goto point3;
    }

    gsize len;
    guint8 *contents = NULL;
    if (!g_file_get_contents(tmp_file, (gchar**) &contents, &len, NULL)) {
        g_printerr("Failed to read temporary file: %s\n", tmp_file);
        g_free(contents);
        goto point3;
    }

    rv = g_bytes_new(contents, len);

    g_free(contents);
point3:
    g_free(stderr_child);
point2:
    if (tmp_file != NULL)
        g_unlink(tmp_file);
    g_free(tmp_file);
    if (tmp_file2 != NULL)
        g_unlink(tmp_file2);
    g_free(tmp_file2);
point1:
    g_free(gcr);
    return rv;
}
