/*
 * balde: A microframework for C based on GLib.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_SAPI_FCGI_PRIVATE_H
#define _BALDE_SAPI_FCGI_PRIVATE_H

#include <glib.h>
#include "../requests.h"

balde_request_env_t* balde_sapi_fcgi_parse_request(GByteArray *params,
    GByteArray *body);
void balde_sapi_fcgi_add_record(GByteArray *ba, guint16 request_id, guint8 type,
    guint8 *data, guint16 data_len);

#endif /* _BALDE_SAPI_FCGI_PRIVATE_H */
