/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_UTILS_PRIVATE_H
#define _BALDE_UTILS_PRIVATE_H

#include <glib.h>

gchar* balde_base64_encode(const guchar *data, gsize len);
guchar* balde_base64_decode(const gchar *text, gsize *out_len);

#endif /* _BALDE_UTILS_PRIVATE_H */
