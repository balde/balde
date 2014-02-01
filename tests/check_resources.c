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
#include <balde/app.h>
#include <balde/resources-private.h>
#include "resources.h"


void
test_resources_list_files(void)
{
    GError *error = NULL;
    gchar **rv = balde_resources_list_files(resources_get_resource(), &error);
    g_assert(rv != NULL);
    g_assert(error == NULL);
    g_assert_cmpstr(rv[0], ==, "/static/lol.css");
    g_assert_cmpstr(rv[1], ==, "/static/lol.js");
    g_assert_cmpstr(rv[2], ==, "/static/zz.js");
    g_assert(rv[3] == NULL);
    g_strfreev(rv);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/resources/list_files", test_resources_list_files);
    return g_test_run();
}
