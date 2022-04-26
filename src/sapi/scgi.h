/*
 * balde: A microframework for C based on GLib.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_SAPI_SCGI_PRIVATE_H
#define _BALDE_SAPI_SCGI_PRIVATE_H

#include <glib.h>
#include <gio/gio.h>

#include "../balde.h"

balde_request_env_t* balde_sapi_scgi_parse_request(balde_app_t *app,
    GInputStream *istream);

#endif /* _BALDE_SAPI_SCGI_PRIVATE_H */
