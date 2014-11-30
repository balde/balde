/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_CGI_PRIVATE_H
#define _BALDE_CGI_PRIVATE_H

#include <glib.h>
#include "balde.h"
#include "wrappers.h"

guint64 balde_cgi_parse_content_length(const gchar *str);
gchar* balde_cgi_stdin_read(balde_app_t *app);
balde_request_env_t* balde_cgi_parse_request(balde_app_t *app);
void balde_cgi_response_print(GString *response);
void balde_cgi_run(balde_app_t *app);

#endif /* _BALDE_CGI_PRIVATE_H */
