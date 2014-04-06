/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <balde-template/parser-private.h>


void
balde_template_free_blocks(GSList *blocks)
{
    balde_template_block_t *block;
    balde_template_include_block_t *iblock;
    balde_template_content_block_t *cblock;
    balde_template_print_var_block_t *vblock;
    balde_template_print_fn_call_block_t *fblock;
    balde_template_fn_arg_t *arg;
    for(GSList *tmp = blocks; tmp != NULL; tmp = g_slist_next(tmp)) {
        block = tmp->data;
        switch(block->type) {
            case BALDE_TEMPLATE_INCLUDE_BLOCK:
                iblock = block->block;
                g_free(iblock->include);
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
    g_slist_free(blocks);
}
