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
#include "../src/utils.h"


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


void
test_timestamp(void)
{
    g_assert(balde_timestamp() == 100000);
}


void
test_encoded_timestamp(void)
{
    gchar *now = balde_encoded_timestamp();
    g_assert_cmpstr(now, ==, "MTAwMDAw");
    g_free(now);
}


void
test_validate_timestamp(void)
{
    // token generated at 99950, "now" is 100000.
    g_assert(balde_validate_timestamp("OTk5NTA=", 100));
    g_assert(balde_validate_timestamp("OTk5NTA=", 50));
    g_assert(!balde_validate_timestamp("OTk5NTA=", 40));

    // guda
    g_assert(!balde_validate_timestamp("Z3VkYQ==", 100));
}


void
test_constant_time_compare(void)
{
    g_assert(balde_constant_time_compare("bola", "bola"));
    g_assert(!balde_constant_time_compare("bola", "guda"));
    g_assert(!balde_constant_time_compare("guda", "gude"));
    g_assert(balde_constant_time_compare("\xd3V\xbeo\xf7\x1d",
        "\xd3V\xbeo\xf7\x1d"));
    g_assert(!balde_constant_time_compare("\xd3V\xbeo\xf7\x1d",
        "\xd3V\xbe\xf7\x1d"));
    g_assert(balde_constant_time_compare("ação", "ação"));
    g_assert(!balde_constant_time_compare("ação", "açã"));
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/utils/base64_encode", test_base64_encode);
    g_test_add_func("/utils/base64_decode", test_base64_decode);
    g_test_add_func("/utils/timestamp", test_timestamp);
    g_test_add_func("/utils/encoded_timestamp", test_encoded_timestamp);
    g_test_add_func("/utils/validate_timestamp", test_validate_timestamp);
    g_test_add_func("/utils/constant_time_compare", test_constant_time_compare);
    return g_test_run();
}
