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
#include "../src/template/template.h"
#include "../src/template/parser.h"
#include "utils.h"


void
balde_assert_template_import(GList *l, const gchar *import)
{
    balde_template_block_t *node = l->data;
    g_assert(node->type == BALDE_TEMPLATE_IMPORT_BLOCK);
    balde_template_import_block_t *block = node->block;
    g_assert_cmpstr(block->import, ==, import);
}


void
balde_assert_template_include(GList *l, const gchar *include)
{
    balde_template_block_t *node = l->data;
    g_assert(node->type == BALDE_TEMPLATE_INCLUDE_BLOCK);
    balde_template_include_block_t *block = node->block;
    g_assert_cmpstr(block->include, ==, include);
}


void
balde_assert_template_if(GList *l, const gchar *if_var)
{
    balde_template_block_t *node = l->data;
    g_assert(node->type == BALDE_TEMPLATE_IF_BLOCK);
    balde_template_if_block_t *block = node->block;
    g_assert_cmpstr(block->if_var, ==, if_var);
}


void
balde_assert_template_else(GList *l)
{
    balde_template_block_t *node = l->data;
    g_assert(node->type == BALDE_TEMPLATE_ELSE_BLOCK);
    g_assert(node->block == NULL);
}


void
balde_assert_template_if_end(GList *l)
{
    balde_template_block_t *node = l->data;
    g_assert(node->type == BALDE_TEMPLATE_IF_END_BLOCK);
    g_assert(node->block == NULL);
}


void
balde_assert_template_content(GList *l, const gchar *content)
{
    balde_template_block_t *node = l->data;
    g_assert(node->type == BALDE_TEMPLATE_CONTENT_BLOCK);
    balde_template_content_block_t *block = node->block;
    g_assert_cmpstr(block->content, ==, content);
}


void
balde_assert_template_print_var(GList *l, const gchar *variable)
{
    balde_template_block_t *node = l->data;
    g_assert(node->type == BALDE_TEMPLATE_PRINT_VAR_BLOCK);
    balde_template_print_var_block_t *block = node->block;
    g_assert_cmpstr(block->variable, ==, variable);
}


void
balde_assert_template_print_function_call(GList *l, const gchar *name, ...)
{
    va_list args;
    va_start(args, name);
    balde_template_block_t *node = l->data;
    g_assert(node->type == BALDE_TEMPLATE_PRINT_FN_CALL_BLOCK);
    balde_template_print_fn_call_block_t *block = node->block;
    g_assert_cmpstr(block->name, ==, name);
    for (GSList *tmp = block->args; tmp != NULL; tmp = g_slist_next(tmp))
        g_assert_cmpstr(((balde_template_fn_arg_t*) tmp->data)->content, ==,
            va_arg(args, const gchar*));
    va_end(args);
}


void
balde_assert_template_equal(const gchar *v1, const gchar *v2)
{
    if (g_strcmp0(v1, v2) != 0) {
        g_print("\n==================================================================\n");
        g_print("%s\n", v1);
        g_print("==================================================================\n");
        g_print("%s\n", v2);
        g_print("==================================================================\n");
        g_assert(FALSE);
    }
}


void
test_template_generate_source_without_vars(void)
{
    gchar *tmpl = get_template("foo.html");
    gchar *exp = get_expected_template("foo.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_single_var(void)
{
    gchar *tmpl = get_template("foo2.html");
    gchar *exp = get_expected_template("foo2.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_single_var_with_imports(void)
{
    gchar *tmpl = get_template("foo3.html");
    gchar *exp = get_expected_template("foo3.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_single_var_with_single_quoted_imports(void)
{
    gchar *tmpl = get_template("foo4.html");
    gchar *exp = get_expected_template("foo4.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_multiple_vars(void)
{
    gchar *tmpl = get_template("foo5.html");
    gchar *exp = get_expected_template("foo5.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_multiple_vars_and_lines(void)
{
    gchar *tmpl = get_template("foo6.html");
    gchar *exp = get_expected_template("foo6.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_multiple_vars_and_lines_with_function_calls(void)
{
    gchar *tmpl = get_template("foo7.html");
    gchar *exp = get_expected_template("foo7.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_multiple_vars_and_lines_with_function_calls_and_single_quotes(void)
{
    gchar *tmpl = get_template("foo8.html");
    gchar *exp = get_expected_template("foo8.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_with_percent(void)
{
    gchar *tmpl = get_template("foo9.html");
    gchar *exp = get_expected_template("foo9.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_with_include(void)
{
    gchar *tmpl = get_template("base.html");
    gchar *exp = get_expected_template("base.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_with_include_with_url_for(void)
{
    gchar *tmpl = get_template("base2.html");
    gchar *exp = get_expected_template("base2.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_with_if(void)
{
    gchar *tmpl = get_template("base3.html");
    gchar *exp = get_expected_template("base3.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_with_multiple_if(void)
{
    gchar *tmpl = get_template("base4.html");
    gchar *exp = get_expected_template("base4.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_with_else(void)
{
    gchar *tmpl = get_template("base5.html");
    gchar *exp = get_expected_template("base5.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_with_include_and_ws_cleaner(void)
{
    gchar *tmpl = get_template("base6.html");
    gchar *exp = get_expected_template("base6.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_with_include_and_ws_cleaner2(void)
{
    gchar *tmpl = get_template("base7.html");
    gchar *exp = get_expected_template("base7.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_source_with_if_and_ws_cleaner(void)
{
    gchar *tmpl = get_template("base8.html");
    gchar *exp = get_expected_template("base8.c");
    gchar *rv = balde_template_generate_source("bola", tmpl);
    g_free(tmpl);
    balde_assert_template_equal(rv, exp);
    g_free(rv);
    g_free(exp);
}


void
test_template_generate_header(void)
{
    gchar *rv = balde_template_generate_header("bola");
    balde_assert_template_equal(rv,
        "// WARNING: this file was generated automatically by balde-template-gen\n"
        "\n"
        "#ifndef __bola_balde_template\n"
        "#define __bola_balde_template\n"
        "\n"
        "#include <balde.h>\n"
        "\n"
        "extern gchar* balde_str_template_bola(balde_app_t *app, "
        "balde_request_t *request, balde_response_t *response);\n"
        "extern void balde_template_bola(balde_app_t *app, balde_request_t *request, "
        "balde_response_t *response);\n"
        "\n"
        "#endif\n");
    g_free(rv);
}


void
test_template_generate_dependencies(void)
{
    gchar *tmpl = get_template("foo.html");
    gchar *rv = balde_template_generate_dependencies(tmpl);
    g_assert_cmpstr(rv, ==, tmpl);
    g_free(tmpl);
    g_free(rv);
}


void
test_template_generate_dependencies_with_include(void)
{
    gchar *tmpl = get_template("base2.html");
    gchar *rv = balde_template_generate_dependencies(tmpl);
    gchar *dir = g_path_get_dirname(tmpl);
    gchar *include = g_build_filename(dir, "body2.html", NULL);
    g_free(dir);
    gchar *dependencies = g_strdup_printf("%s %s", tmpl, include);
    g_free(include);
    g_free(tmpl);
    g_assert_cmpstr(rv, ==, dependencies);
    g_free(dependencies);
    g_free(rv);
}


void
test_template_get_name(void)
{
    gchar *rv = balde_template_get_name("bola.guda.ação.html");
    g_assert_cmpstr(rv, ==, "bola_guda_a____o");
    g_free(rv);
}


void
test_template_parse(void)
{
    GList *blocks = balde_template_parse(
        "Test\n"
        "\n"
        "   {%- import \"bola.h\" %}\n"
        "foo {{ bola(gude, 'asd', \"zxc\", TRUE, NULL, 1.5, -2, \"chun\\\"da\", 'gu\\'da', FALSE, xd, +5, \"\") }}\n"
        "  {{ test }} \n"
        "    {% include 'foo.html' -%}\n"
        "{% if chunda %}\n"
        "bola\n"
        "{% else %}\n"
        "guda\n"
        "{% endif %}\n"
        "{% import 'asd.h' %}");
    g_assert(blocks != NULL);
    balde_assert_template_content(blocks, "Test");
    balde_assert_template_import(blocks->next, "bola.h");
    balde_assert_template_content(blocks->next->next, "\nfoo ");
    balde_assert_template_print_function_call(blocks->next->next->next, "bola",
        "gude", "\"asd\"", "\"zxc\"", "TRUE", "NULL", "1.5", "-2",
        "\"chun\\\"da\"", "\"gu'da\"", "FALSE", "xd", "+5", "\"\"");
    balde_assert_template_content(blocks->next->next->next->next, "\n  ");
    balde_assert_template_print_var(blocks->next->next->next->next->next, "test");
    balde_assert_template_content(blocks->next->next->next->next->next->next,
        " \n    ");
    balde_assert_template_include(blocks->next->next->next->next->next->next->next,
        "foo.html");
    balde_assert_template_content(blocks->next->next->next->next->next->next->next->next,
        "");
    balde_assert_template_if(blocks->next->next->next->next->next->next->next->next->next,
        "chunda");
    balde_assert_template_content(blocks->next->next->next->next->next->next->next->next->next->next,
        "\nbola\n");
    balde_assert_template_else(blocks->next->next->next->next->next->next->next->next->next->next->next);
    balde_assert_template_content(blocks->next->next->next->next->next->next->next->next->next->next->next->next,
        "\nguda\n");
    balde_assert_template_if_end(blocks->next->next->next->next->next->next->next->next->next->next->next->next->next);
    balde_assert_template_content(blocks->next->next->next->next->next->next->next->next->next->next->next->next->next->next,
        "\n");
    balde_assert_template_import(blocks->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next,
        "asd.h");
    g_assert(blocks->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next == NULL);
    balde_template_free_blocks(blocks);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/template/generate_source_without_vars",
        test_template_generate_source_without_vars);
    g_test_add_func("/template/generate_source_single_var",
        test_template_generate_source_single_var);
    g_test_add_func("/template/generate_source_single_var_with_imports",
        test_template_generate_source_single_var_with_imports);
    g_test_add_func("/template/generate_source_single_var_with_single_quoted_imports",
        test_template_generate_source_single_var_with_single_quoted_imports);
    g_test_add_func("/template/generate_source_multiple_vars",
        test_template_generate_source_multiple_vars);
    g_test_add_func("/template/generate_source_multiple_vars_and_lines",
        test_template_generate_source_multiple_vars_and_lines);
    g_test_add_func("/template/generate_source_multiple_vars_and_lines_with_function_calls",
        test_template_generate_source_multiple_vars_and_lines_with_function_calls);
    g_test_add_func("/template/generate_source_multiple_vars_and_lines_with_function_calls_and_single_quotes",
        test_template_generate_source_multiple_vars_and_lines_with_function_calls_and_single_quotes);
    g_test_add_func("/template/generate_source_with_percent",
        test_template_generate_source_with_percent);
    g_test_add_func("/template/generate_source_with_include",
        test_template_generate_source_with_include);
    g_test_add_func("/template/generate_source_with_include_with_url_for",
        test_template_generate_source_with_include_with_url_for);
    g_test_add_func("/template/generate_source_with_if",
        test_template_generate_source_with_if);
    g_test_add_func("/template/generate_source_with_multiple_if",
        test_template_generate_source_with_multiple_if);
    g_test_add_func("/template/generate_source_with_else",
        test_template_generate_source_with_else);
    g_test_add_func("/template/generate_source_with_include_and_ws_cleaner",
        test_template_generate_source_with_include_and_ws_cleaner);
    g_test_add_func("/template/generate_source_with_include_and_ws_cleaner2",
        test_template_generate_source_with_include_and_ws_cleaner2);
    g_test_add_func("/template/generate_source_with_if_and_ws_cleaner",
        test_template_generate_source_with_if_and_ws_cleaner);
    g_test_add_func("/template/generate_header",
        test_template_generate_header);
    g_test_add_func("/template/generate_dependencies",
        test_template_generate_dependencies);
    g_test_add_func("/template/generate_dependencies_with_include",
        test_template_generate_dependencies_with_include);
    g_test_add_func("/template/get_name", test_template_get_name);
    g_test_add_func("/template/parse", test_template_parse);
    return g_test_run();
}
