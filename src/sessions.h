/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_SESSIONS_PRIVATE_H
#define _BALDE_SESSIONS_PRIVATE_H

#include <glib.h>

typedef enum {
    BALDE_SESSION_UNSIGN_OK,
    BALDE_SESSION_UNSIGN_BAD_FORMAT,
    BALDE_SESSION_UNSIGN_BAD_SIGN,
    BALDE_SESSION_UNSIGN_BAD_TIMESTAMP,
} balde_session_unsign_status_t;

typedef struct {
    GHashTable *storage;
    gint64 max_age;
    gchar *key;
} balde_session_t;

gchar* balde_session_serialize(GHashTable *session);
GHashTable* balde_session_unserialize(const gchar* text);
gchar* balde_session_derive_key(const guchar *key, gsize key_len);
gchar* balde_session_sign(const guchar *key, gsize key_len,
    const gchar *content);
balde_session_unsign_status_t balde_session_unsign(const guchar *key,
    gsize key_len, guint max_age, const gchar *signed_str, gchar **content);

#endif /* _BALDE_SESSIONS_PRIVATE_H */
