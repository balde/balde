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
#include "parser.h"


void
balde_unescape_single_quoted_strings(gchar** str)
{
    GRegex *re_escaped_str = g_regex_new("\\\\'", 0, 0, NULL);
    gchar *escaped_str = g_regex_replace_literal(re_escaped_str, *str, -1, 0,
        "'", 0, NULL);
    g_free(*str);
    *str = escaped_str;
    g_regex_unref(re_escaped_str);
}


void
balde_template_free_blocks(GList *blocks)
{
    balde_template_block_t *block;
    balde_template_import_block_t *imblock;
    balde_template_include_block_t *iblock;
    balde_template_if_block_t *ifblock;
    balde_template_content_block_t *cblock;
    balde_template_print_var_block_t *vblock;
    balde_template_print_fn_call_block_t *fblock;
    balde_template_fn_arg_t *arg;
    for(GList *tmp = blocks; tmp != NULL; tmp = g_list_next(tmp)) {
        block = tmp->data;
        switch(block->type) {
            case BALDE_TEMPLATE_IMPORT_BLOCK:
                imblock = block->block;
                g_free(imblock->import);
                break;
            case BALDE_TEMPLATE_INCLUDE_BLOCK:
                iblock = block->block;
                g_free(iblock->include);
                break;
            case BALDE_TEMPLATE_IF_BLOCK:
                ifblock = block->block;
                g_free(ifblock->if_var);
                break;
            case BALDE_TEMPLATE_ELSE_BLOCK:
            case BALDE_TEMPLATE_IF_END_BLOCK:
                break;
            case BALDE_TEMPLATE_CONTENT_BLOCK:
                cblock = block->block;
                g_free(cblock->content);
                break;
            case BALDE_TEMPLATE_PRINT_VAR_BLOCK:
                vblock = block->block;
                g_free(vblock->variable);
                break;
            case BALDE_TEMPLATE_PRINT_FN_CALL_BLOCK:
                fblock = block->block;
                g_free(fblock->name);
                for(GSList *tmp2 = fblock->args; tmp2 != NULL; tmp2 = g_slist_next(tmp2)) {
                    arg = tmp2->data;
                    g_free(arg->content);
                    g_free(arg);
                }
                g_slist_free(fblock->args);
                break;
        }
        g_free(block->block);
        g_free(block);
    }
    g_list_free(blocks);
}
