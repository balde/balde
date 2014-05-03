/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_TEMPLATE_PARSER_H
#define _BALDE_TEMPLATE_PARSER_H

#include <glib.h>

typedef struct {
    gchar *include;
} balde_template_include_block_t;

typedef struct {
    gchar *content;
} balde_template_content_block_t;

typedef struct {
    gchar *variable;
} balde_template_print_var_block_t;

typedef enum {
    BALDE_TEMPLATE_FN_ARG_STRING,
    BALDE_TEMPLATE_FN_ARG_VAR,
    BALDE_TEMPLATE_FN_ARG_INT,
    BALDE_TEMPLATE_FN_ARG_FLOAT,
    BALDE_TEMPLATE_FN_ARG_BOOL,
    BALDE_TEMPLATE_FN_ARG_NULL,
} balde_template_fn_arg_type_t;

typedef struct {
    gchar *content;
    balde_template_fn_arg_type_t type;
} balde_template_fn_arg_t;

typedef struct {
    gchar *name;
    GSList *args;
} balde_template_print_fn_call_block_t;

typedef enum {
    BALDE_TEMPLATE_INCLUDE_BLOCK,
    BALDE_TEMPLATE_CONTENT_BLOCK,
    BALDE_TEMPLATE_PRINT_VAR_BLOCK,
    BALDE_TEMPLATE_PRINT_FN_CALL_BLOCK,
} balde_template_block_type_t;

typedef struct {
    balde_template_block_type_t type;
    gpointer block;
} balde_template_block_t;

GSList* balde_template_parse(const gchar *tmpl);
void balde_template_free_blocks(GSList *blocks);

#endif /* _BALDE_TEMPLATE_PARSER_H */
