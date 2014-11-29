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
#include <balde/app.h>
#include <balde/app-private.h>
#include <balde/wrappers.h>


/**
 * \example hello-with-url_for.c
 *
 * An example with the url_for helper. It depends on more files. Take a look at
 * it on the balde source code.
 */

gchar*
balde_tmpl_url_for(balde_app_t *app, balde_request_t *request,
    const gchar *endpoint, gboolean external, ...)
{
    va_list params;
    va_start(params, external);
    gchar *rv = balde_app_url_forv(app, endpoint, params);
    va_end(params);
    return rv;
}
