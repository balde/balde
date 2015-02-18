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
#include "balde.h"
#include "balde-private.h"
#include "app.h"


BALDE_API gchar*
balde_tmpl_url_for(balde_app_t *app, balde_request_t *request,
    const gchar *endpoint, gboolean external, ...)
{
    va_list params;
    va_start(params, external);
    gchar *rv = balde_app_url_forv(app, request, endpoint, params);
    va_end(params);
    return rv;
}
