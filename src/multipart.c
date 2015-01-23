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
#include <string.h>
#include "balde.h"
#include "multipart.h"
#include "multipart_parser.h"
#include "requests.h"


gchar*
balde_multipart_parse_boundary(const gchar *header)
{
    if (header == NULL)
        return NULL;
    gchar *boundary = NULL;
    gchar **pieces = g_strsplit(header, ";", 0);
    for (guint i = 0; pieces[i] != NULL; i++) {
        gchar *opt = g_strstrip(pieces[i]);
        gchar **opt_pieces = g_strsplit(opt, "=", 2);
        if (g_strv_length(opt_pieces) != 2)
            goto clean;
        if (g_strcmp0(opt_pieces[0], "boundary") != 0)
            goto clean;
        gchar *unquoted_boundary = g_shell_unquote(opt_pieces[1], NULL); // wtf
        boundary = g_strdup_printf("--%s", unquoted_boundary);
        g_free(unquoted_boundary);
clean:
        g_strfreev(opt_pieces);
    }
    g_strfreev(pieces);
    return boundary;
}


static int
read_header_name(multipart_parser *p, const char *at, size_t length)
{
    balde_multipart_state_t *state = multipart_parser_get_data(p);
    gchar *header_name = g_strndup(at, length);
    state->header_name = g_ascii_strdown(header_name, -1);
    g_free(header_name);
    return 0;
}


static int
read_header_value(multipart_parser *p, const char *at, size_t length)
{
    balde_multipart_state_t *state = multipart_parser_get_data(p);
    if (state->header_name == NULL)
        return 1;
    if (g_strcmp0(state->header_name, "content-type") == 0) {
        state->file_type = g_strndup(at, length);
    }
    else if (g_strcmp0(state->header_name, "content-disposition") == 0) {
        if (g_str_has_prefix(at, "form-data;")) {  // other types aren't supported
            gchar *header_value = g_strndup(at, length);
            gchar **pieces = g_strsplit(header_value, ";", 0);
            g_free(header_value);
            for (guint i = 0; pieces[i] != NULL; i++) {
                gchar *opt = g_strstrip(pieces[i]);
                gchar **opt_pieces = g_strsplit(opt, "=", 2);
                if (g_strv_length(opt_pieces) != 2)
                    goto clean;
                if (g_strcmp0(opt_pieces[0], "filename") == 0)
                    state->file_name = g_shell_unquote(opt_pieces[1], NULL); // wtf
                else if (g_strcmp0(opt_pieces[0], "name") == 0)
                    state->field_name = g_shell_unquote(opt_pieces[1], NULL); // wtf
clean:
                g_strfreev(opt_pieces);
            }
            g_strfreev(pieces);
        }
    }
    g_free(state->header_name);
    state->header_name = NULL;
    return 0;
}


static int
read_part_data_begin(multipart_parser *p)
{
    balde_multipart_state_t *state = multipart_parser_get_data(p);
    if (state == NULL)
        return 1;

    state->file_content = g_string_new(NULL);

    return 0;
}


static int
read_part_data_end(multipart_parser *p)
{
    balde_multipart_state_t *state = multipart_parser_get_data(p);
    if (state == NULL || state->field_name == NULL)
        return 1;

    if (state->file_name != NULL && state->file_type != NULL) {
        balde_file_t *file = g_new(balde_file_t, 1);
        file->name = g_strdup(state->file_name);
        file->content = state->file_content;
        file->type = g_strdup(state->file_type);
        g_hash_table_replace(state->data->files, g_strdup(state->field_name),
            file);
    }
    else {
        g_hash_table_replace(state->data->form, g_strdup(state->field_name),
            g_string_free(state->file_content, FALSE));
    }

    g_free(state->file_name);
    g_free(state->file_type);
    g_free(state->field_name);
    state->file_name = NULL;
    state->file_type = NULL;
    state->field_name = NULL;
    state->file_content = NULL;

    return 0;
}


static int
read_part_data(multipart_parser *p, const char *at, size_t length)
{
    balde_multipart_state_t *state = multipart_parser_get_data(p);
    if (state == NULL)
        return 1;

    g_string_append_len(state->file_content, at, length);

    return 0;
}


balde_multipart_data_t*
balde_multipart_parse(const gchar *boundary, const GString *body)
{
    if (boundary == NULL)
        return NULL;

    multipart_parser_settings callbacks;
    memset(&callbacks, 0, sizeof(multipart_parser_settings));

    callbacks.on_header_field = read_header_name;
    callbacks.on_header_value = read_header_value;
    callbacks.on_part_data = read_part_data;
    callbacks.on_part_data_begin = read_part_data_begin;
    callbacks.on_part_data_end = read_part_data_end;

    multipart_parser* parser = multipart_parser_init(boundary, &callbacks);

    balde_multipart_state_t *state = g_new(balde_multipart_state_t, 1);
    state->header_name = NULL;
    state->file_name = NULL;
    state->file_type = NULL;
    state->field_name = NULL;
    state->data = g_new(balde_multipart_data_t, 1);
    state->data->files = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
        (GDestroyNotify) balde_file_free);
    state->data->form = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
        g_free);
    multipart_parser_set_data(parser, state);

    if (multipart_parser_execute(parser, body->str, body->len) != body->len) {
        g_hash_table_destroy(state->data->files);
        g_hash_table_destroy(state->data->form);
        g_free(state->data);
        state->data = NULL;
    }

    multipart_parser_free(parser);

    g_free(state->header_name);
    g_free(state->file_name);
    g_free(state->file_type);
    g_free(state->field_name);
    balde_multipart_data_t *data = state->data;
    g_free(state);

    return data;
}
