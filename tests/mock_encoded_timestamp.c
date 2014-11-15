/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>

extern gboolean valid_timestamp;


// this is a poor man's mock of balde_encoded_timestamp :)
gchar*
balde_encoded_timestamp(void)
{
    return g_strdup("MTAwMDAw");
}


// this is a poor man's mock of balde_validate_timestamp :)
gboolean
balde_validate_timestamp(const gchar* timestamp, gint64 max_delta)
{
    return valid_timestamp;
}

#include "../libbalde/sessions.c"
