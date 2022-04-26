/*
 * balde: A microframework for C based on GLib.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_SAPI_PRIVATE_H
#define _BALDE_SAPI_PRIVATE_H

#include <glib.h>
#include "balde.h"

typedef GOptionGroup* (*balde_sapi_init_func_t) (void);
typedef gboolean (*balde_sapi_supported_func_t) (void);
typedef gint (*balde_sapi_run_func_t) (balde_app_t*);

typedef struct {
    const char *name;
    balde_sapi_init_func_t init;
    balde_sapi_supported_func_t supported;
    balde_sapi_run_func_t run;
} balde_sapi_t;

void balde_sapi_init(GOptionContext *context);
gint balde_sapi_run(balde_app_t *app, GOptionContext *context);

#endif /* _BALDE_SAPI_PRIVATE_H */
