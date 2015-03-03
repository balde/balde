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
#include <stdio.h>
#include <string.h>
#include "../src/cgi.h"

// this thing isn't thread safe, bla, bla, bla, but this is just a test :)
extern const gchar *query_string;
static guint counter = 0;


// this is a poor man's mock of getc :)
int
getc(FILE *stream)
{
    int c = (int) query_string[counter++];
    if (counter == strlen(query_string))
        counter = 0;
    return c;
}

#include "../src/cgi.c"
