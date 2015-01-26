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
#include "balde.h"
#include "balde-private.h"
#include "value.h"

// this thing is "similar", but way simpler than GValue


BALDE_API balde_value_t*
balde_value_null(void)
{
    balde_value_t *rv = g_new(balde_value_t, 1);
    rv->type = BALDE_VALUE_TYPE_NULL;
    rv->value = NULL;
    return rv;
}


BALDE_API balde_value_t*
balde_value_string(const gchar *value)
{
    balde_value_t *rv = g_new(balde_value_t, 1);
    rv->type = BALDE_VALUE_TYPE_STRING;
    rv->value = g_strdup(value);
    return rv;
}


#define GENERATE_VALUE_FUNC(type_, flag)                                     \
    BALDE_API balde_value_t*                                                 \
    balde_value_##type_(g##type_ value)                                      \
    {                                                                        \
        balde_value_t *rv = g_new(balde_value_t, 1);                         \
        rv->type = flag;                                                     \
        g##type_ *v = g_new(g##type_, 1);                                    \
        *v = value;                                                          \
        rv->value = v;                                                       \
        return rv;                                                           \
    }

GENERATE_VALUE_FUNC(boolean, BALDE_VALUE_TYPE_BOOLEAN)
GENERATE_VALUE_FUNC(char, BALDE_VALUE_TYPE_CHAR)
GENERATE_VALUE_FUNC(uchar, BALDE_VALUE_TYPE_UCHAR)
GENERATE_VALUE_FUNC(int, BALDE_VALUE_TYPE_INT)
GENERATE_VALUE_FUNC(uint, BALDE_VALUE_TYPE_UINT)
GENERATE_VALUE_FUNC(long, BALDE_VALUE_TYPE_LONG)
GENERATE_VALUE_FUNC(ulong, BALDE_VALUE_TYPE_ULONG)
GENERATE_VALUE_FUNC(int64, BALDE_VALUE_TYPE_INT64)
GENERATE_VALUE_FUNC(uint64, BALDE_VALUE_TYPE_UINT64)
GENERATE_VALUE_FUNC(float, BALDE_VALUE_TYPE_FLOAT)
GENERATE_VALUE_FUNC(double, BALDE_VALUE_TYPE_DOUBLE)

#undef GENERATE_VALUE_FUNC


BALDE_API gchar*
balde_value_to_string(balde_value_t *value)
{
    if (value == NULL)
        return NULL;

    switch (value->type) {
        case BALDE_VALUE_TYPE_NULL:
            return NULL;
        case BALDE_VALUE_TYPE_STRING:
            return g_strdup((gchar*) value->value);
        case BALDE_VALUE_TYPE_BOOLEAN:
            if (*((gboolean*) value->value))
                return g_strdup("TRUE");
            return g_strdup("FALSE");
        case BALDE_VALUE_TYPE_CHAR:
            return g_strdup_printf("%c", *((gchar*) value->value));
        case BALDE_VALUE_TYPE_UCHAR:
            return g_strdup_printf("%c", *((guchar*) value->value));
        case BALDE_VALUE_TYPE_INT:
            return g_strdup_printf("%d", *((gint*) value->value));
        case BALDE_VALUE_TYPE_UINT:
            return g_strdup_printf("%u", *((guint*) value->value));
        case BALDE_VALUE_TYPE_LONG:
            return g_strdup_printf("%ld", *((glong*) value->value));
        case BALDE_VALUE_TYPE_ULONG:
            return g_strdup_printf("%lu", *((gulong*) value->value));
        case BALDE_VALUE_TYPE_INT64:
            return g_strdup_printf("%" G_GINT64_FORMAT, *((gint64*) value->value));
        case BALDE_VALUE_TYPE_UINT64:
            return g_strdup_printf("%" G_GUINT64_FORMAT, *((guint64*) value->value));
        case BALDE_VALUE_TYPE_FLOAT:
            return g_strdup_printf("%f", *((gfloat*) value->value));
        case BALDE_VALUE_TYPE_DOUBLE:
            return g_strdup_printf("%f", *((gdouble*) value->value));
    }
}


BALDE_API void
balde_value_free(balde_value_t *value)
{
    if (value == NULL)
        return;
    g_free(value->value);
    g_free(value);
    value = NULL;
}
