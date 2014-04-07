/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_TEMPLATE_HELPERS_H
#define _BALDE_TEMPLATE_HELPERS_H

#include <glib.h>
#include <balde/app.h>
#include <balde/wrappers.h>

gchar* balde_tmpl_url_for(balde_app_t *app, balde_request_t *request,
    const gchar *endpoint, gboolean external, ...);

#endif /* _BALDE_TEMPLATE_HELPERS_H */
