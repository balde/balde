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
#include "../src/balde.h"
#include "../src/fcgi.h"


void
test_fcgi_parse_headers(void)
{
    const gchar *tmp[] = {
        "HTTP_LOL_HEHE=12345",
        "HTTP_XD_KKK=asdf",
        NULL
    };
    GHashTable *headers = balde_fcgi_request_headers(tmp);
    g_assert(g_hash_table_size(headers) == 2);
    g_assert_cmpstr(g_hash_table_lookup(headers, "lol-hehe"), ==, "12345");
    g_assert_cmpstr(g_hash_table_lookup(headers, "xd-kkk"), ==, "asdf");
    g_hash_table_destroy(headers);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/fcgi/parse_headers", test_fcgi_parse_headers);
    return g_test_run();
}
