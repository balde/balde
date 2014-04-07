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

/** Template helper functions.
 *
 * These functions are designed to be called from the templates. When calling
 * them from a template, the user should omit the 'balde_tmpl_' prefix and
 * the two first arguments: application context and request context, that are
 * filled automatically by balde.
 *
 * \file balde/template-helpers.h
 */

/** Template helper to get the URL for a given endpoint.
 *
 * You should pass the exact number of parameters expected by the view's URL
 * rule.
 *
 * The 'external' parameter isn't implemented yet, but is planned on the API.
 * If set to TRUE, the function will return an absolute URL.
 */
gchar* balde_tmpl_url_for(balde_app_t *app, balde_request_t *request,
    const gchar *endpoint, gboolean external, ...);

#endif /* _BALDE_TEMPLATE_HELPERS_H */
