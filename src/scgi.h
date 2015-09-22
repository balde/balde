/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_SCGI_PRIVATE_H
#define _BALDE_SCGI_PRIVATE_H

#include <glib.h>
#include <gio/gio.h>

#include "balde.h"

balde_request_env_t* balde_scgi_parse_request(balde_app_t *app,
    GInputStream *istream);
void balde_scgi_run(balde_app_t *app, const gchar *host, gint16 port,
    gint max_threads);

#endif /* _BALDE_SCGI_PRIVATE_H */
