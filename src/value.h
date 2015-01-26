/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_VALUE_PRIVATE_H
#define _BALDE_VALUE_PRIVATE_H

#include <glib.h>

typedef enum {
    BALDE_VALUE_TYPE_NULL,
    BALDE_VALUE_TYPE_BOOLEAN,
    BALDE_VALUE_TYPE_CHAR,
    BALDE_VALUE_TYPE_UCHAR,
    BALDE_VALUE_TYPE_INT,
    BALDE_VALUE_TYPE_UINT,
    BALDE_VALUE_TYPE_LONG,
    BALDE_VALUE_TYPE_ULONG,
    BALDE_VALUE_TYPE_INT64,
    BALDE_VALUE_TYPE_UINT64,
    BALDE_VALUE_TYPE_FLOAT,
    BALDE_VALUE_TYPE_DOUBLE,
    BALDE_VALUE_TYPE_STRING,
} balde_value_type_t;

struct _balde_value_t {
    gpointer value;
    balde_value_type_t type;
};

#endif /* _BALDE_VALUE_PRIVATE_H */
