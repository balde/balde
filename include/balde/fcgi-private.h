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
#include <balde/app.h>

typedef struct {
    const gchar *host;
    gint16 port;
    balde_app_t *app;
    guint id;
} balde_fcgi_thread_ctx_t;

typedef struct {
    FCGX_Stream *in;
    FCGX_Stream *out;
    FCGX_Stream *err;
    gchar **envp;
} balde_fcgi_request_t;

GHashTable* balde_fcgi_request_headers(const gchar **envp);
void balde_fcgi_run(balde_app_t *app, const gchar *host, gint16 port, gint max_threads);

#endif /* _BALDE_FCGI_PRIVATE_H */
