/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_RESPONSES_PRIVATE_H
#define _BALDE_RESPONSES_PRIVATE_H

#include <glib.h>
#include "balde.h"

struct _balde_response_private_t {
    GHashTable *headers;
    GHashTable *template_ctx;
    GString *body;
};

void balde_response_headers_free(gpointer l);
void balde_response_free(balde_response_t *response);
balde_response_t* balde_make_response_from_gstring(GString *content);
balde_response_t* balde_make_response_from_exception(GError *error);
void balde_fix_header_name(gchar *name);
void balde_header_render(const gchar *key, GSList *value, GString *str);
gchar* balde_response_generate_etag(balde_response_t *response, gboolean weak);
GString* balde_response_render(balde_response_t *response,
    const gboolean with_body);
void balde_response_print(GString *response);

#endif /* _BALDE_RESPONSES_PRIVATE_H */
