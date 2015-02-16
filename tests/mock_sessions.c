/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>

extern guint64 timestamp;


// this is a poor man's mock of g_date_time_new_now_utc :)
GDateTime*
g_date_time_new_now_utc(void)
{
    return g_date_time_new_from_unix_utc(timestamp);
}

#include "../src/utils.c"
