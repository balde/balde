/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_CGI_H
#define _BALDE_CGI_H

#include <glib.h>

#ifdef ENABLE_FASTCGI
#  include <fcgi_stdio.h>
#  define BEGIN_LOOP while (FCGI_Accept() >= 0) {
#  define END_LOOP }
#else
#  include <stdio.h>
#  define BEGIN_LOOP do {
#  define END_LOOP } while(0);
#endif

void balde_stdout_handler(const gchar *str);
void balde_stderr_handler(const gchar *str);

#endif /* _BALDE_CGI_H */
