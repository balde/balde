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


balde_app*
balde_app_init(void)
{
    balde_app *app = g_new(balde_app, 1);
    app->views = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
        (GDestroyNotify) balde_app_free);
    // TODO: load config from somewhere
    app->config = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    app->error = NULL;
    return app;
}


void
balde_app_free(balde_app *app)
{
    g_hash_table_destroy(app->views);
    g_hash_table_destroy(app->config);
    if (app->error != NULL)
        g_error_free(app->error);
    g_free(app);
}
