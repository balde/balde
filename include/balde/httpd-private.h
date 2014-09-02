/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_HTTPD_PRIVATE_H
#define _BALDE_HTTPD_PRIVATE_H

#include <glib.h>
#include <balde/app.h>
#include <balde/wrappers-private.h>

typedef struct {
    balde_request_env_t *request;
    GString *body;
    gchar *header_key;
} balde_httpd_parser_data_t;

balde_request_env_t* balde_httpd_parse_request(GString *request);
void balde_httpd_run(balde_app_t *app, const gchar *host, gint16 port);
GString* balde_httpd_response_render(balde_response_t *response, const gboolean with_body);

#endif /* _BALDE_HTTPD_PRIVATE_H */
