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
#include "../src/resources.h"
#include "../src/multipart.h"
#include "utils.h"


void
balde_assert_resource(const balde_resource_t *resource, gchar *name,
    gchar *content, gchar *type, gchar *hash_name, gchar *hash_content)
{
    if (name != NULL)
        g_assert_cmpstr(resource->name, ==, name);
    else
        g_assert(resource->name == NULL);
    if (content != NULL)
        g_assert_cmpstr(resource->content->str, ==, content);
    else
        g_assert(resource->content == NULL);
    if (type != NULL)
        g_assert_cmpstr(resource->type, ==, type);
    else
        g_assert(resource->type == NULL);
    if (hash_name != NULL)
        g_assert_cmpstr(resource->hash_name, ==, hash_name);
    else
        g_assert(resource->hash_name == NULL);
    if (hash_content != NULL)
        g_assert_cmpstr(resource->hash_content, ==, hash_content);
    else
        g_assert(resource->hash_content == NULL);
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

    const balde_resource_t *file = g_hash_table_lookup(data->files, "bola");
    g_assert(file != NULL);
    balde_assert_resource(file, "foo.txt", "This is a random file!!!!\n\n:D\n",
        "text/plain", "4fd8cc85ca9eebd2fa3c550069ce2846",
        "d4278ab98e74c3009f849e155fa2ee51");

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

    const balde_resource_t *file = g_hash_table_lookup(data->files, "bola");
    g_assert(file != NULL);
    balde_assert_resource(file, "foo.txt", "This is a random file!!!!\n\n:D\n",
        "text/plain", "4fd8cc85ca9eebd2fa3c550069ce2846",
        "d4278ab98e74c3009f849e155fa2ee51");

    file = g_hash_table_lookup(data->files, "bola2");
    g_assert(file != NULL);
    balde_assert_resource(file, "foo2.txt", "This is another random file!!!!\n\n:D\n",
        "text/plain", "5e6caa8011f4387069acd5c3ebecf488",
        "449916399a7a33dc2739a961ed7f9ac2");

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
