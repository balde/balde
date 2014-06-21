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
#include <balde/utils-private.h>


void
test_base64_encode(void)
{
    gchar *t = balde_base64_encode((const guchar*) "\xd3V\xbeo\xf7\x1d", 6);
    g_assert_cmpstr(t, ==, "01a-b_cd");
    g_free(t);
}


void
test_base64_decode(void)
{
    gsize len;
    guchar *t = balde_base64_decode("01a-b_cd", &len);
    gchar *v = g_strndup((const gchar*) t, len);
    g_assert(len == 6);
    g_assert_cmpstr((gchar*) v, ==, "\xd3V\xbeo\xf7\x1d");
    g_free(v);
    g_free(t);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/utils/base64_encode", test_base64_encode);
    g_test_add_func("/utils/base64_decode", test_base64_decode);
    return g_test_run();
}
