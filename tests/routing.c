#include "lib/routing.h"

#include <glib.h>


void
test_url_match()
{
    g_assert(url_match("/lol/", "/lol/") != NULL);
    g_assert(g_hash_table_size(url_match("/lol/", "/lol/")) == 0);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/routing/url_match", test_url_match);
    return g_test_run();
}
