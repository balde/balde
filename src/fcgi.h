/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_FCGI_PRIVATE_H
#define _BALDE_FCGI_PRIVATE_H

#include <glib.h>
#include <fcgiapp.h>
#include "balde.h"

typedef struct {
    balde_app_t *app;
    gint sock;
} balde_fcgi_ctx_t;

GHashTable* balde_fcgi_request_headers(const gchar **envp);
void balde_fcgi_run(balde_app_t *app, const gchar *host, gint16 port,
    gint max_threads, gint backlog, gboolean listen);

#endif /* _BALDE_FCGI_PRIVATE_H */
