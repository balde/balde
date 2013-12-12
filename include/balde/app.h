/**
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_APP_H
#define _BALDE_APP_H

#include <glib.h>

typedef struct {
	GHashTable *views;
	GHashTable *config;
	GError *error;
} balde_app;

balde_app* balde_app_init(void);
void balde_app_free(balde_app *app);

#endif /* _BALDE_APP_H */
