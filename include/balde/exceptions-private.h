/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_EXCEPTIONS_PRIVATE_H
#define _BALDE_EXCEPTIONS_PRIVATE_H

#include <glib.h>
#include <balde/exceptions.h>

typedef struct {
    const balde_http_exception_code_t code;
    const gchar *name;
    const gchar *description;
} balde_http_exception_t;

const gchar* balde_exception_get_name_from_code(const balde_http_exception_code_t code);
const gchar* balde_exception_get_description_from_code(const balde_http_exception_code_t code);

#endif /* _BALDE_EXCEPTIONS_PRIVATE_H */
