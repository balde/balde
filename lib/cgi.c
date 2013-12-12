/**
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <balde/cgi.h>


void
balde_stdout_handler(const gchar *str)
{
    fprintf(stdout, "%s", (char*) str);
}


void
balde_stderr_handler(const gchar *str)
{
    fprintf(stderr, "%s", (char*) str);
}
