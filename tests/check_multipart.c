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
#include "../src/balde.h"
#include "../src/resources.h"
#include "../src/multipart.h"
#include "utils.h"


void
balde_assert_file(const balde_file_t *file, gchar *name, gchar *content,
    gchar *type)
{
    if (name != NULL)
        g_assert_cmpstr(file->name, ==, name);
    else
        g_assert(file->name == NULL);
    if (content != NULL)
        g_assert_cmpstr(file->content->str, ==, content);
    else
        g_assert(file->content == NULL);
    if (type != NULL)
        g_assert_cmpstr(file->type, ==, type);
    else
        g_assert(file->type == NULL);
}


void
test_multipart_parse_boundary(void)
{
    gchar *boundary = balde_multipart_parse_boundary(
        "Does-Not-Matters; boundary=--------bola");
    g_assert_cmpstr(boundary, ==, "----------bola");
    g_free(boundary);
    boundary = balde_multipart_parse_boundary(
        "Does-Not-Matters; boundary=\"--------bola\"");
    g_assert_cmpstr(boundary, ==, "----------bola");
    g_free(boundary);
    boundary = balde_multipart_parse_boundary(
        "Does-Not-Matters; random_arg=guda; boundary=--------bola");
    g_assert_cmpstr(boundary, ==, "----------bola");
    g_free(boundary);
    boundary = balde_multipart_parse_boundary(
        "Does-Not-Matters; random_arg=guda; boundary=--------bola; asd=2");
    g_assert_cmpstr(boundary, ==, "----------bola");
    g_free(boundary);
    boundary = balde_multipart_parse_boundary("Does-Not-Matters");
    g_assert(boundary == NULL);
}


void
test_multipart_parse(void)
{
    GString *up = get_upload("simple.txt");
    balde_multipart_data_t *data = balde_multipart_parse(
        "-----------------------------12056991879709948637505812", up);
    g_string_free(up, TRUE);

    g_assert(data != NULL);
    g_assert_cmpint(g_hash_table_size(data->files), ==, 1);
    g_assert_cmpint(g_hash_table_size(data->form), ==, 1);

    const balde_file_t *file = g_hash_table_lookup(data->files, "bola");
    g_assert(file != NULL);
    balde_assert_file(file, "foo.txt", "This is a random file!!!!\n\n:D\n",
        "text/plain");

    const gchar *form = g_hash_table_lookup(data->form, "name");
    g_assert_cmpstr(form, ==, "chunda");

    g_hash_table_destroy(data->files);
    g_hash_table_destroy(data->form);
    g_free(data);
}


void
test_multipart_parse_without_filename(void)
{
    GString *up = get_upload("without-filename.txt");
    balde_multipart_data_t *data = balde_multipart_parse(
        "-----------------------------12056991879709948637505812", up);
    g_string_free(up, TRUE);

    g_assert(data != NULL);
    g_assert_cmpint(g_hash_table_size(data->files), ==, 0);
    g_assert_cmpint(g_hash_table_size(data->form), ==, 2);

    g_assert(g_hash_table_lookup(data->files, "bola") == NULL);

    const gchar *form = g_hash_table_lookup(data->form, "name");
    g_assert_cmpstr(form, ==, "chunda");

    form = g_hash_table_lookup(data->form, "bola");
    g_assert_cmpstr(form, ==, "This is a random file!!!!\n\n:D\n");

    g_hash_table_destroy(data->files);
    g_hash_table_destroy(data->form);
    g_free(data);
}


void
test_multipart_parse_without_filetype(void)
{
    GString *up = get_upload("without-filetype.txt");
    balde_multipart_data_t *data = balde_multipart_parse(
        "-----------------------------12056991879709948637505812", up);
    g_string_free(up, TRUE);

    g_assert(data != NULL);
    g_assert_cmpint(g_hash_table_size(data->files), ==, 0);
    g_assert_cmpint(g_hash_table_size(data->form), ==, 2);

    g_assert(g_hash_table_lookup(data->files, "bola") == NULL);

    const gchar *form = g_hash_table_lookup(data->form, "name");
    g_assert_cmpstr(form, ==, "chunda");

    form = g_hash_table_lookup(data->form, "bola");
    g_assert_cmpstr(form, ==, "This is a random file!!!!\n\n:D\n");

    g_hash_table_destroy(data->files);
    g_hash_table_destroy(data->form);
    g_free(data);
}


void
test_multipart_parse_multiple(void)
{
    GString *up = get_upload("multiple.txt");
    balde_multipart_data_t *data = balde_multipart_parse(
        "-----------------------------12056991879709948637505812", up);
    g_string_free(up, TRUE);

    g_assert(data != NULL);
    g_assert_cmpint(g_hash_table_size(data->files), ==, 2);
    g_assert_cmpint(g_hash_table_size(data->form), ==, 2);

    const balde_file_t *file = g_hash_table_lookup(data->files, "bola");
    g_assert(file != NULL);
    balde_assert_file(file, "foo.txt", "This is a random file!!!!\n\n:D\n",
        "text/plain");

    file = g_hash_table_lookup(data->files, "bola2");
    g_assert(file != NULL);
    balde_assert_file(file, "foo2.txt", "This is another random file!!!!\n\n:D\n",
        "text/plain");

    const gchar *form = g_hash_table_lookup(data->form, "name");
    g_assert_cmpstr(form, ==, "chunda");

    form = g_hash_table_lookup(data->form, "name2");
    g_assert_cmpstr(form, ==, "chunda2");

    g_hash_table_destroy(data->files);
    g_hash_table_destroy(data->form);
    g_free(data);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/multipart/parse_boundary", test_multipart_parse_boundary);
    g_test_add_func("/multipart/parse", test_multipart_parse);
    g_test_add_func("/multipart/parse_without_filename",
        test_multipart_parse_without_filename);
    g_test_add_func("/multipart/parse_without_filetype",
        test_multipart_parse_without_filetype);
    g_test_add_func("/multipart/parse_multiple", test_multipart_parse_multiple);
    return g_test_run();
}
