/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_RESOURCES_PRIVATE_H
#define _BALDE_RESOURCES_PRIVATE_H

#include <glib.h>
#include <gio/gio.h>
#include <balde/app.h>

gchar** balde_resources_list_files(balde_app_t *app);

#endif /* _BALDE_RESOURCES_PRIVATE_H */
