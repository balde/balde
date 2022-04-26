/*
 * balde: A microframework for C based on GLib.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_SAPI_CGI_PRIVATE_H
#define _BALDE_SAPI_CGI_PRIVATE_H

#include <glib.h>
#include "../balde.h"
#include "../requests.h"
#include "../sapi.h"

guint64 balde_sapi_cgi_parse_content_length(const gchar *str);
GString* balde_sapi_cgi_stdin_read(balde_app_t *app);
GHashTable* balde_sapi_cgi_request_headers(void);
balde_request_env_t* balde_sapi_cgi_parse_request(balde_app_t *app);
void balde_sapi_cgi_response_print(GString *response);
gint balde_sapi_cgi_run(balde_app_t *app);

#endif /* _BALDE_SAPI_CGI_PRIVATE_H */
