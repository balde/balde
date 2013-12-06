/**
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include "routing.h"


void
test_url_match(void)
{
    GHashTable *matches = url_match("/lol/", "/lol/");
    g_assert(matches != NULL);
    g_assert(g_hash_table_size(matches) == 0);
    g_hash_table_destroy(matches);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/routing/url_match", test_url_match);
    return g_test_run();
}
