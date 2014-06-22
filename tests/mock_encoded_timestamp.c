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


// this is a poor man's mock of balde_encoded_timestamp :)
gchar*
balde_encoded_timestamp(void)
{
    return g_strdup("MTAwMDAw");
}

#include "../libbalde/sessions.c"
