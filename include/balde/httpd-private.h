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
#include <balde/wrappers-private.h>

typedef struct {
    balde_request_env_t *request;
    GString *body;
    gchar *header_key;
} balde_httpd_parser_data_t;

balde_request_env_t* balde_httpd_parse_request(const gchar* request, gsize len);

#endif /* _BALDE_HTTPD_PRIVATE_H */
