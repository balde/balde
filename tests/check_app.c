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
#include <balde/app.h>


void
test_app_init(void)
{
    balde_app *app = balde_app_init();
    g_assert(app != NULL);
    g_assert(app->config != NULL);
    g_assert(app->views != NULL);
    g_assert(app->error == NULL);
    balde_app_free(app);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/app/init", test_app_init);
    return g_test_run();
}
