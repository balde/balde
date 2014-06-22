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
#include <string.h>
#include <json-glib/json-glib.h>
#include <balde/utils-private.h>


static void
balde_session_serialize_hash_table_item(gchar* key, gchar* value,
    JsonBuilder *builder)
{
    json_builder_set_member_name(builder, key);
    json_builder_add_string_value(builder, value);
}


gchar*
balde_session_serialize(GHashTable *session)
{
    JsonBuilder *builder = json_builder_new();
    json_builder_begin_object(builder);
    g_hash_table_foreach(session,
        (GHFunc) balde_session_serialize_hash_table_item, builder);
    json_builder_end_object(builder);
    JsonGenerator *gen = json_generator_new();
    JsonNode *root = json_builder_get_root(builder);
    json_generator_set_root(gen, root);
    gsize len;
    guchar *str = (guchar*) json_generator_to_data(gen, &len);
    json_node_free(root);
    g_object_unref(gen);
    g_object_unref(builder);
    gchar *rv = balde_base64_encode(str, len);
    g_free(str);
    return rv;
}


static void
balde_session_unserialize_hash_table_item(JsonObject *object,
    const gchar *member_name, JsonNode *member_node, GHashTable **session)
{
    if (*session == NULL)
        return;
    gchar *member_value;
    switch (JSON_NODE_TYPE(member_node)) {
        case JSON_NODE_NULL:
            member_value = NULL;
            break;
        case JSON_NODE_VALUE:
            member_value = json_node_dup_string(member_node);
            if (member_value == NULL) {
                // ooooops... value isn't a string!
                g_hash_table_destroy(*session);
                *session = NULL;
                return;
            }
            break;
        default:
            return;
    }
    g_hash_table_insert(*session, g_strdup(member_name), member_value);
}


GHashTable*
balde_session_unserialize(const gchar* text)
{
    GHashTable *session = NULL;
    JsonParser *parser = json_parser_new();
    gsize len;
    gchar* json = (gchar*) balde_base64_decode(text, &len);
    if (!json_parser_load_from_data(parser, json, len, NULL))
        goto point1;
    JsonNode *root = json_parser_get_root(parser);
    if (JSON_NODE_TYPE(root) != JSON_NODE_OBJECT)
        goto point1;
    JsonObject *obj = json_node_get_object(root);
    if (obj == NULL)
        goto point1;
    session = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    json_object_foreach_member(obj,
        (JsonObjectForeach) balde_session_unserialize_hash_table_item, &session);
point1:
    g_object_unref(parser);
    g_free(json);
    return session;
}


gchar*
balde_session_sign(const guchar *key, gsize key_len, const gchar *content)
{
    // content should be nul-terminated.
    // FIXME: key should/could be derived.
    gchar *sign = g_compute_hmac_for_string(G_CHECKSUM_SHA1, key, key_len,
        content, strlen(content));
    gchar *timestamp = balde_encoded_timestamp();
    gchar *rv = g_strdup_printf("%s.%s.%s", content, timestamp, sign);
    g_free(timestamp);
    g_free(sign);
    return rv;
}
