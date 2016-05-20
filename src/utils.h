/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_UTILS_PRIVATE_H
#define _BALDE_UTILS_PRIVATE_H

#include <glib.h>

// Tue Jan  1 00:00:00 UTC 2013
#define BALDE_EPOCH 1356998400

gchar* balde_base64_encode(const guchar *data, gsize len);
guchar* balde_base64_decode(const gchar *text, gsize *out_len);
gint64 balde_timestamp(void);
gchar* balde_encoded_timestamp(void);
gboolean balde_validate_timestamp(const gchar* timestamp, gint64 max_delta);
gboolean balde_constant_time_compare(const gchar *v1, const gchar *v2);

#endif /* _BALDE_UTILS_PRIVATE_H */
