/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_REQUESTS_PRIVATE_H
#define _BALDE_REQUESTS_PRIVATE_H

#include <glib.h>
#include "balde.h"
#include "sessions.h"

typedef struct {
    gchar *request_method;
    gchar *server_name;
    gchar *script_name;
    gchar *path_info;
    gchar *query_string;
    GHashTable *headers;
    GString *body;
    gboolean https;
} balde_request_env_t;

struct _balde_request_private_t {
    GHashTable *args;
    GHashTable *form;
    GHashTable *files;
    GHashTable *view_args;
    GHashTable *headers;
    GHashTable *cookies;
    GString *body;
    balde_session_t *session;
};

gchar* balde_parse_header_name_from_envvar(const gchar *env_name);
gchar* balde_urldecode(const gchar* str);
GHashTable* balde_parse_query_string(const gchar *query_string);
GHashTable* balde_parse_cookies(const gchar *cookie_header);
balde_authorization_t* balde_parse_authorization(const gchar *authorization);
void balde_authorization_free(balde_authorization_t *authorization);
balde_request_t* balde_make_request(balde_app_t *app, balde_request_env_t *env);
void balde_request_free(balde_request_t *request);
void balde_request_env_free(balde_request_env_t *request);
void balde_file_free(balde_file_t *file);

#endif /* _BALDE_WRAPPERS_PRIVATE_H */
