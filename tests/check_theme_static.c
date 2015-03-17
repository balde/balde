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
#include <gio/gio.h>
#include "../src/theme/static.h"
#include "utils.h"


void
test_theme_static_generate_resource_xml(void)
{
    gchar *files[4];
    guint i = 0;
    files[i++] = "bola.css";
    files[i++] = "guda.jpg";
    files[i++] = "chaves.png";
    files[i++] = NULL;
    gchar *xml = balde_theme_static_generate_resource_xml(files);
    g_assert_cmpstr(xml, ==,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<gresources>\n"
        "    <gresource prefix=\"/static\">\n"
        "        <file>bola.css</file>\n"
        "        <file>guda.jpg</file>\n"
        "        <file>chaves.png</file>\n"
        "    </gresource>\n"
        "</gresources>\n");
    g_free(xml);
}


void
test_theme_static_get_resource_data(void)
{
    gchar *files[4];
    guint i = 0;
    files[i++] = "lol.css";
    files[i++] = "lol.js";
    files[i++] = "zz.sh";
    files[i++] = NULL;
    gchar *static_dir = get_static_dir();
    GBytes *data = balde_theme_static_get_resource_data(static_dir, files);
    g_free(static_dir);

    GResource *res = g_resource_new_from_data(data, NULL);
    g_assert(res != NULL);

    gchar **childrens = g_resource_enumerate_children(res, "/static", 0, NULL);
    g_assert(childrens != NULL);
    g_assert_cmpint(g_strv_length(childrens), ==, 3);
    g_strfreev(childrens);

    GBytes *r1 = g_resource_lookup_data(res, "/static/lol.css", 0, NULL);
    g_assert(r1 != NULL);
    g_assert_cmpstr(g_bytes_get_data(r1, NULL), ==,
        "body {\n"
        "    background-color: #CCC;\n"
        "}\n");
    g_bytes_unref(r1);

    GBytes *r2 = g_resource_lookup_data(res, "/static/lol.js", 0, NULL);
    g_assert(r2 != NULL);
    g_assert_cmpstr(g_bytes_get_data(r2, NULL), ==,
        "function a() {\n"
        "    alert('lol');\n"
        "}\n");
    g_bytes_unref(r2);

    GBytes *r3 = g_resource_lookup_data(res, "/static/zz.sh", 0, NULL);
    g_assert(r3 != NULL);
    g_assert_cmpstr(g_bytes_get_data(r3, NULL), ==,
        "#!/bin/bash\n"
        "\n"
        "zz() {\n"
        "    :\n"
        "}\n");
    g_bytes_unref(r3);


    g_resource_unref(res);
    g_bytes_unref(data);
}


void
test_theme_static_render_resource_data(void)
{
    const guint8 t[5] = "bola";
    GBytes *b = g_bytes_new(t, 5);
    gchar *tmp = balde_theme_static_render_resource_data(b);
    g_assert_cmpstr(tmp, ==, "{0x62, 0x6f, 0x6c, 0x61, 0x00}");
    g_free(tmp);
    g_bytes_unref(b);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/theme_static/generate_resource_xml",
        test_theme_static_generate_resource_xml);
    g_test_add_func("/theme_static/get_resource_data",
        test_theme_static_get_resource_data);
    g_test_add_func("/theme_static/render_resource_data",
        test_theme_static_render_resource_data);
    return g_test_run();
}
