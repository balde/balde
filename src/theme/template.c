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
#include <stdlib.h>
#include <string.h>
#include "template.h"
#include "parser.h"


void
balde_template_build_state(const gchar *filename, balde_template_state_t **state)
{
    if (*state == NULL) {
        *state = g_new(balde_template_state_t, 1);
        (*state)->includes = NULL;
        (*state)->imports = NULL;
        (*state)->body = g_string_new("");
        (*state)->indent = 4;
        (*state)->declare_tmp = FALSE;
    }

    gchar *dirname = g_path_get_dirname(filename);
    gchar *template_source;
    if (!g_file_get_contents(filename, &template_source, NULL, NULL))
        g_error("Failed to read source file: %s\n", filename);
    GList *blocks = balde_template_parse(template_source);
    g_free(template_source);

    gchar *tmp_str;

    for (GList *tmp = blocks; tmp != NULL; tmp = g_list_next(tmp)) {
        balde_template_block_t *node = tmp->data;
        switch (node->type) {
            case BALDE_TEMPLATE_IMPORT_BLOCK:
                (*state)->imports = g_slist_append((*state)->imports,
                    g_strdup(((balde_template_import_block_t*) node->block)->import));
                break;
            case BALDE_TEMPLATE_INCLUDE_BLOCK:
                tmp_str = g_build_filename(dirname,
                    ((balde_template_include_block_t*) node->block)->include,
                    NULL);
                (*state)->includes = g_slist_append((*state)->includes, tmp_str);
                balde_template_build_state(tmp_str, state);
                break;
            case BALDE_TEMPLATE_CONTENT_BLOCK:
                tmp_str = g_strescape(((balde_template_content_block_t*) node->block)->content, "");
                g_string_append_printf((*state)->body,
                    "%*sg_string_append(rv, \"%s\");\n", (*state)->indent, "",
                    tmp_str);
                g_free(tmp_str);
                break;
            case BALDE_TEMPLATE_PRINT_VAR_BLOCK:
                g_string_append_printf((*state)->body,
                    "%*sg_string_append(rv, balde_response_get_tmpl_var_or_empty(response, \"%s\"));\n",
                    (*state)->indent, "",
                    ((balde_template_print_var_block_t*) node->block)->variable);
                break;
            case BALDE_TEMPLATE_IF_BLOCK:
                g_string_append_printf((*state)->body,
                    "%*sif (balde_response_get_tmpl_var(response, \"%s\") != NULL) {\n",
                    (*state)->indent, "",
                    ((balde_template_if_block_t*) node->block)->if_var);
                (*state)->indent += 4;
                break;
            case BALDE_TEMPLATE_ELSE_BLOCK:
                g_string_append_printf((*state)->body,
                    "%*s}\n"
                    "%*selse {\n", (*state)->indent - 4, "", (*state)->indent - 4, "");
                break;
            case BALDE_TEMPLATE_IF_END_BLOCK:
                (*state)->indent -= 4;
                g_string_append_printf((*state)->body, "%*s}\n", (*state)->indent, "");
                break;
            case BALDE_TEMPLATE_PRINT_FN_CALL_BLOCK:
                (*state)->declare_tmp = TRUE;
                g_string_append_printf((*state)->body,
                    "%*stmp = balde_tmpl_%s(app, request", (*state)->indent,
                    "", ((balde_template_print_fn_call_block_t*) node->block)->name);
                if (((balde_template_print_fn_call_block_t*) node->block)->args != NULL)
                    g_string_append((*state)->body, ", ");
                else
                    g_string_append_printf((*state)->body,
                        ");\n"
                        "%*sg_string_append(rv, tmp);\n"
                        "%*sg_free(tmp);\n", (*state)->indent, "",
                        (*state)->indent, "");
                for (GSList *tmp2 = ((balde_template_print_fn_call_block_t*) node->block)->args;
                        tmp2 != NULL; tmp2 = g_slist_next(tmp2)) {
                    switch (((balde_template_fn_arg_t*) tmp2->data)->type) {
                        case BALDE_TEMPLATE_FN_ARG_STRING:
                        case BALDE_TEMPLATE_FN_ARG_INT:
                        case BALDE_TEMPLATE_FN_ARG_FLOAT:
                        case BALDE_TEMPLATE_FN_ARG_BOOL:
                        case BALDE_TEMPLATE_FN_ARG_NULL:
                            g_string_append((*state)->body,
                                ((balde_template_fn_arg_t*) tmp2->data)->content);
                            break;
                        case BALDE_TEMPLATE_FN_ARG_VAR:
                            g_string_append_printf((*state)->body,
                                "balde_response_get_tmpl_var(response, \"%s\")",
                                ((balde_template_fn_arg_t*) tmp2->data)->content);
                            break;
                    }
                    if (g_slist_next(tmp2) == NULL)
                        g_string_append_printf((*state)->body,
                            ");\n"
                            "%*sg_string_append(rv, tmp);\n"
                            "%*sg_free(tmp);\n", (*state)->indent, "",
                            (*state)->indent, "");
                    else
                        g_string_append((*state)->body, ", ");
                }
                break;
        }
    }

    balde_template_free_blocks(blocks);
    g_free(dirname);
}


void
balde_template_free_state(balde_template_state_t *state)
{
    if (state == NULL)
        return;
    g_slist_free_full(state->includes, g_free);
    g_slist_free_full(state->imports, g_free);
    g_string_free(state->body, TRUE);
    g_free(state);
}


gchar*
balde_template_generate_source(const gchar *template_name, const gchar *file_name)
{
    balde_template_state_t *state = NULL;
    balde_template_build_state(file_name, &state);

    GString *rv = g_string_new(
        "// WARNING: this file was generated automatically by balde-template-gen\n"
        "\n"
        "#include <balde.h>\n"
        "#include <glib.h>\n");

    for (GSList *tmp = state->imports; tmp != NULL; tmp = g_slist_next(tmp))
        g_string_append_printf(rv, "#include <%s>\n", (gchar*) tmp->data);

    g_string_append_printf(rv,
        "\n"
        "extern gchar* balde_str_template_%s(balde_app_t *app, "
        "balde_request_t *request, balde_response_t *response);\n"
        "extern void balde_template_%s(balde_app_t *app, balde_request_t *request, "
        "balde_response_t *response);\n"
        "\n"
        "gchar*\n"
        "balde_str_template_%s(balde_app_t *app, balde_request_t *request, "
        "balde_response_t *response)\n"
        "{\n"
        "    GString *rv = g_string_new(\"\");\n",
        template_name, template_name, template_name);

    if (state->declare_tmp)
        g_string_append(rv, "    gchar *tmp;\n");

    g_string_append_len(rv, state->body->str, state->body->len);

    g_string_append_printf(rv,
        "    return g_string_free(rv, FALSE);\n"
        "}\n"
        "\n"
        "void\n"
        "balde_template_%s(balde_app_t *app, balde_request_t *request, "
        "balde_response_t *response)\n"
        "{\n"
        "    gchar *rv = balde_str_template_%s(app, request, response);\n"
        "    balde_response_append_body(response, rv);\n"
        "    g_free(rv);\n"
        "}\n", template_name, template_name);

    balde_template_free_state(state);

    return g_string_free(rv, FALSE);
}


gchar*
balde_template_generate_header(const gchar *template_name)
{
    return g_strdup_printf(
        "// WARNING: this file was generated automatically by balde-template-gen\n"
        "\n"
        "#ifndef __%s_balde_template\n"
        "#define __%s_balde_template\n"
        "\n"
        "#include <balde.h>\n"
        "\n"
        "extern gchar* balde_str_template_%s(balde_app_t *app, "
        "balde_request_t *request, balde_response_t *response);\n"
        "extern void balde_template_%s(balde_app_t *app, balde_request_t *request, "
        "balde_response_t *response);\n"
        "\n"
        "#endif\n", template_name, template_name, template_name, template_name);
}


gchar*
balde_template_generate_dependencies(const gchar *file_name)
{
    balde_template_state_t *state = NULL;
    balde_template_build_state(file_name, &state);

    gchar **dependencies = g_new(gchar*, g_slist_length(state->includes) + 2);
    dependencies[0] = g_strdup(file_name);

    guint i = 1;
    for (GSList *tmp = state->includes; tmp != NULL; tmp = g_slist_next(tmp))
        dependencies[i++] = g_strdup((gchar*) tmp->data);
    dependencies[i] = NULL;

    gchar *str = g_strjoinv(" ", dependencies);

    g_strfreev(dependencies);
    balde_template_free_state(state);

    return str;
}


gchar*
balde_template_get_name(const gchar *template_basename)
{
    gchar *template_name = g_path_get_basename(template_basename);
    for (guint i = strlen(template_name); i != 0; i--) {
        if (template_name[i] == '.') {
            template_name[i] = '\0';
            break;
        }
    }
    for (guint i = 0; template_name[i] != '\0'; i++) {
        if (!g_ascii_isalpha(template_name[i])) {
            template_name[i] = '_';
        }
    }
    return template_name;
}
