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
#include <json-glib/json-glib.h>
#include "config.h"


static gchar**
get_array(JsonReader *reader, const gchar *key)
{
    GSList *l = NULL;
    if (json_reader_read_member(reader, key)) {
        if (json_reader_is_array(reader)) {
            gint len = json_reader_count_elements(reader);
            for (guint i = 0; i < len; i++) {
                if (json_reader_read_element(reader, i)) {
                    if (json_reader_is_value(reader)) {
                        JsonNode *node = json_reader_get_value(reader);
                        if (json_node_get_value_type(node) == G_TYPE_STRING)
                            l = g_slist_append(l, json_node_dup_string(node));
                    }
                    json_reader_end_element(reader);
                }
            }
        }
        json_reader_end_member(reader);
    }
    gchar **rv = g_new(gchar*, g_slist_length(l) + 1);
    guint i = 0;
    for (GSList *tmp = l; tmp != NULL; tmp = g_slist_next(tmp))
        rv[i++] = tmp->data;
    rv[i] = NULL;
    return rv;
}


balde_theme_config_t*
balde_theme_config_read(const gchar *filepath)
{
    balde_theme_config_t *rv = NULL;
    JsonParser *parser = json_parser_new();
    GError *err = NULL;
    if (!json_parser_load_from_file(parser, filepath, &err)) {
        g_printerr("Failed to parse JSON file: %s\n", err->message);
        g_error_free(err);
        goto point1;
    }
    JsonNode *root = json_parser_get_root(parser);
    if (JSON_NODE_TYPE(root) != JSON_NODE_OBJECT)
        goto point1;

    JsonReader *reader = json_reader_new(root);

    rv = g_new(balde_theme_config_t, 1);
    rv->templates = get_array(reader, "templates");
    rv->static_resources = get_array(reader, "static");

    g_object_unref(reader);
point1:
    g_object_unref(parser);
    return rv;
}
