/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_WRAPPERS_PRIVATE_H
#define _BALDE_WRAPPERS_PRIVATE_H

#include <glib.h>
#include "balde.h"

typedef struct {
    gchar *request_method;
    gchar *script_name;
    gchar *path_info;
    gchar *query_string;
    GHashTable *headers;
    guint64 content_length;
    gchar *body;
} balde_request_env_t;

struct _balde_request_private_t {
    GHashTable *args;
    GHashTable *form;
    GHashTable *view_args;
    GHashTable *headers;
    GHashTable *cookies;
    GString *body;
};

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
GString* balde_response_render(balde_response_t *response,
    const gboolean with_body);
void balde_response_print(GString *response);
gchar* balde_parse_header_name_from_envvar(const gchar *env_name);
GHashTable* balde_request_headers(void);
gchar* balde_urldecode(const gchar* str);
GHashTable* balde_parse_query_string(const gchar *query_string);
GHashTable* balde_parse_cookies(const gchar *cookie_header);
balde_authorization_t* balde_parse_authorization(const gchar *authorization);
void balde_authorization_free(balde_authorization_t *authorization);
balde_request_t* balde_make_request(balde_app_t *app, balde_request_env_t *env);
void balde_request_free(balde_request_t *request);
void balde_request_env_free(balde_request_env_t *request);

#endif /* _BALDE_WRAPPERS_PRIVATE_H */
