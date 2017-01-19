/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include "sapi.h"

extern balde_sapi_t fcgi_sapi;
extern balde_sapi_t scgi_sapi;
extern balde_sapi_t httpd_sapi;
extern balde_sapi_t cgi_sapi;


// *any* new server api should be added here, otherwise it won't get initialized.
// they should be also sorted by preference. CGI should be always the last one.
static balde_sapi_t *sapis[] = {
    &fcgi_sapi,
    &scgi_sapi,
    &httpd_sapi,
    &cgi_sapi,
    NULL,
};


void
balde_sapi_init(GOptionContext *context)
{
    for (gsize i = 0; sapis[i] != NULL; i++) {
        if (sapis[i]->init != NULL) {
            GOptionGroup *group = sapis[i]->init();
            if (group != NULL) {
                g_option_context_add_group(context, group);
            }
        }
    }
}


gint
balde_sapi_run(balde_app_t *app, GOptionContext *context)
{
    GSList *supported_sapis = NULL;
    for (gsize i = 0; sapis[i] != NULL; i++) {
        if (sapis[i]->supported != NULL && sapis[i]->supported()) {
            supported_sapis = g_slist_append(supported_sapis, sapis[i]);
        }
    }

    if (supported_sapis == NULL) {
        gchar *help_str = g_option_context_get_help(context, FALSE, NULL);
        g_printerr("%s", help_str);
        g_free(help_str);
        return 3;
    }

    // always use the first sapi supported
    balde_sapi_t *sapi = supported_sapis->data;

    if (sapi->run != NULL)
        return sapi->run(app);

    return 3;
}
