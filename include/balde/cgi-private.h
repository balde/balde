/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_CGI_PRIVATE_H
#define _BALDE_CGI_PRIVATE_H

#include <glib.h>
#include <balde/app.h>

#include <fcgi_stdio.h>
#define BEGIN_LOOP while (FCGI_Accept() >= 0) {
#define END_LOOP }

void balde_stderr_handler(const gchar *str);
gchar* balde_stdin_read(balde_app_t *app);

#endif /* _BALDE_CGI_PRIVATE_H */
