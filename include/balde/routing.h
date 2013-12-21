/**
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_ROUTING_H
#define _BALDE_ROUTING_H

#include <glib.h>

typedef enum {
    BALDE_HTTP_NONE = 0x00,
    BALDE_HTTP_OPTIONS = 0x01,
    BALDE_HTTP_GET = 0x02,
    BALDE_HTTP_HEAD = 0x04,
    BALDE_HTTP_POST = 0x08,
    BALDE_HTTP_PUT = 0x10,
    BALDE_HTTP_DELETE = 0x20
} balde_http_method_t;

typedef struct {
    const gchar *endpoint;
    const gchar *rule;
    balde_http_method_t method;
} balde_url_rule_t;

gboolean balde_url_match(gchar *path, const gchar *rule, GHashTable **matches);
gchar* balde_dispatch_from_path(GSList *views, gchar *path, GHashTable **matches);
gchar* balde_list_allowed_methods(balde_http_method_t method);

#endif /* _BALDE_ROUTING_H */
