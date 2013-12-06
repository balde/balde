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

typedef struct {
    const gchar *endpoint;
    const gchar *rule;
} balde_url_rule_t;

gboolean balde_url_match(gchar *path, const gchar *rule, GHashTable **matches);
gchar* balde_dispatch_from_path(const balde_url_rule_t *rules,
    gchar *path, GHashTable **matches);

#endif /* _BALDE_ROUTING_H */
