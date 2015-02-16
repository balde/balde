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
#include "../src/balde.h"
#include "../src/datetime.h"


void
test_datetime_rfc6265(void)
{
    GDateTime *dt = g_date_time_new_from_unix_utc(1234567890);
    gchar *tmp = balde_datetime_rfc6265(dt);
    g_date_time_unref(dt);
    g_assert_cmpstr(tmp, ==, "Fri, 13-Feb-2009 23:31:30 GMT");
    g_free(tmp);
}


void
test_datetime_rfc5322(void)
{
    GDateTime *dt = g_date_time_new_from_unix_utc(1234567890);
    gchar *tmp = balde_datetime_rfc5322(dt);
    g_date_time_unref(dt);
    g_assert_cmpstr(tmp, ==, "Fri, 13 Feb 2009 23:31:30 GMT");
    g_free(tmp);
}


void
test_datetime_logging(void)
{
    GDateTime *dt = g_date_time_new_from_unix_utc(1234567890);
    gchar *tmp = balde_datetime_logging(dt);
    g_date_time_unref(dt);
    g_assert_cmpstr(tmp, ==, "13/Feb/2009 23:31:30");
    g_free(tmp);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/datetime/rfc6265", test_datetime_rfc6265);
    g_test_add_func("/datetime/rfc5322", test_datetime_rfc5322);
    g_test_add_func("/datetime/logging", test_datetime_logging);
    return g_test_run();
}
