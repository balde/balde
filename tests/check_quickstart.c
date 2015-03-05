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
#include "../src/quickstart/quickstart.h"


void
test_quickstart_get_app_name(void)
{
    gchar *tmp = balde_quickstart_get_app_name("/home/foo/bola");
    g_assert_cmpstr(tmp, ==, "bola");
    g_free(tmp);
    tmp = balde_quickstart_get_app_name(".");
    g_assert_cmpstr(tmp, ==, "unnamed");
    g_free(tmp);
    tmp = balde_quickstart_get_app_name("/");
    g_assert_cmpstr(tmp, ==, "unnamed");
    g_free(tmp);
    tmp = balde_quickstart_get_app_name(NULL);
    g_assert_cmpstr(tmp, ==, "unnamed");
    g_free(tmp);
}


void
test_quickstart_fix_app_name(void)
{
    gchar *tmp = balde_quickstart_fix_app_name("bola", '-');
    g_assert_cmpstr(tmp, ==, "bola");
    g_free(tmp);
    tmp = balde_quickstart_fix_app_name("bola   dsgfds   dgfdsg", '-');
    g_assert_cmpstr(tmp, ==, "bola---dsgfds---dgfdsg");
    g_free(tmp);
    tmp = balde_quickstart_fix_app_name("bola   dsgfds   dgfd~~##", '_');
    g_assert_cmpstr(tmp, ==, "bola___dsgfds___dgfd____");
    g_free(tmp);
    tmp = balde_quickstart_fix_app_name(NULL, '_');
    g_assert(tmp == NULL);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/quickstart/get_app_name", test_quickstart_get_app_name);
    g_test_add_func("/quickstart/fix_app_name", test_quickstart_fix_app_name);
    return g_test_run();
}
