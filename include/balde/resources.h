/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_RESOURCES_H
#define _BALDE_RESOURCES_H

/** Static resources related definitions.
 *
 * \file balde/resources.h
 */

#include <glib.h>
#include <gio/gio.h>
#include <balde/app.h>

/** Load static resources
 *
 * This function loads all the resources found in a GResource object into
 * the application context.
 */
void balde_resources_load(balde_app_t *app, GResource *resources);

/** Initialize a response context with a static resource.
 *
 * The function will return a response object with the static resource content
 * and mime type. If the resource isn't found, a 404 response will be returned.
 */
balde_response_t* balde_make_response_from_static_resource(balde_app_t *app,
    const gchar *name);

#endif /* _BALDE_RESOURCES_H */
