/**
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _ROUTING_H
#define _ROUTING_H

#include <glib.h>

gboolean balde_url_match(gchar *path, gchar *rule, GHashTable **matches);

#endif /* _ROUTING_H */
