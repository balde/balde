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
#include <json-glib/json-glib.h>
#include "balde.h"
#include "balde-private.h"
#include "utils.h"
#include "sessions.h"
#include "requests.h"
#include "responses.h"


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
balde_session_derive_key(const guchar *key, gsize key_len)
{
    return g_compute_hmac_for_string(G_CHECKSUM_SHA1, key, key_len,
        "balde-session-cookie", 20);
}


gchar*
balde_session_sign(const guchar *key, gsize key_len, const gchar *content)
{
    // content should be nul-terminated.
    gchar *timestamp = balde_encoded_timestamp();
    gchar *content_with_ts = g_strdup_printf("%s|%s", content, timestamp);
    gchar *sign = g_compute_hmac_for_string(G_CHECKSUM_SHA1, key, key_len,
        content_with_ts, strlen(content_with_ts));
    gchar *rv = g_strdup_printf("%s.%s", content_with_ts, sign);
    g_free(timestamp);
    g_free(content_with_ts);
    g_free(sign);
    return rv;
}


balde_session_unsign_status_t
balde_session_unsign(const guchar *key, gsize key_len, guint max_age,
    const gchar *signed_str, gchar **content)
{
    *content = NULL;
    balde_session_unsign_status_t rv = BALDE_SESSION_UNSIGN_OK;
    gchar **pieces = g_strsplit(signed_str, ".", 2);
    if (g_strv_length(pieces) != 2) {
        rv = BALDE_SESSION_UNSIGN_BAD_FORMAT;
        goto point1;
    }
    gchar *sign = g_compute_hmac_for_string(G_CHECKSUM_SHA1, key, key_len,
        pieces[0], strlen(pieces[0]));
    if (!balde_constant_time_compare(pieces[1], sign)) {
        rv = BALDE_SESSION_UNSIGN_BAD_SIGN;
        goto point2;
    }
    gchar **content_pieces = g_strsplit(pieces[0], "|", 2);
    if (g_strv_length(content_pieces) != 2) {
        rv = BALDE_SESSION_UNSIGN_BAD_FORMAT;
        goto point3;
    }
    if (!balde_validate_timestamp(content_pieces[1], max_age)) {
        rv = BALDE_SESSION_UNSIGN_BAD_TIMESTAMP;
        goto point3;
    }
    *content = g_strdup(content_pieces[0]);
point3:
    g_strfreev(content_pieces);
point2:
    g_free(sign);
point1:
    g_strfreev(pieces);
    return rv;
}


BALDE_API void
balde_session_open(balde_app_t *app, balde_request_t *request)
{
    if (app->error != NULL || request->priv->session != NULL)
        return;
    request->priv->session = g_new(balde_session_t, 1);
    request->priv->session->storage = NULL;
    request->priv->session->key = NULL;

    // verify session lifetime
    const gchar *session_lifetime = balde_app_get_config(app,
        "PERMANENT_SESSION_LIFETIME");
    request->priv->session->max_age = 31 * 24 * 60 * 60;  // 31 days
    if (session_lifetime != NULL) {
        request->priv->session->max_age = g_ascii_strtoll(session_lifetime,
            NULL, 10);
    }

    // verify if secret_key is set
    const gchar *key = balde_app_get_config(app, "SECRET_KEY");
    if (key == NULL) {
        balde_abort_set_error_with_description(app, 500,
            "To be able to use sessions you need to set the SECRET_KEY "
            "configuration parameter in your application.");
        g_free(request->priv->session->storage);
        g_free(request->priv->session);
        request->priv->session = NULL;
        return;
    }

    // verify if secret_key length is set manually, otherwise defaults to strlen
    const gchar *secret_key_len_str = balde_app_get_config(app, "SECRET_KEY_LENGTH");
    gint key_len;
    if (secret_key_len_str != NULL) {
        key_len = atoi(secret_key_len_str);
        if (key_len < 0)
            key_len = strlen(key);
    }
    else
        key_len = strlen(key);

    // derive key
    request->priv->session->key = balde_session_derive_key((const guchar*) key,
        key_len);

    // verify if we have the cookie
    const gchar *cookie = balde_request_get_cookie(request, "balde_session");
    if (cookie == NULL)
        return;

    gchar *signed_cookie;
    balde_session_unsign_status_t status = balde_session_unsign(
        (const guchar*) request->priv->session->key,
        strlen(request->priv->session->key), request->priv->session->max_age,
        cookie, &signed_cookie);

    // FIXME: tests! status codes are tested, but the code below, no.
    switch (status) {
        case BALDE_SESSION_UNSIGN_BAD_FORMAT:
        case BALDE_SESSION_UNSIGN_BAD_SIGN:
        case BALDE_SESSION_UNSIGN_BAD_TIMESTAMP:
            return;
        case BALDE_SESSION_UNSIGN_OK:
            break;
    }

    request->priv->session->storage = balde_session_unserialize(signed_cookie);

    g_free(signed_cookie);
}


BALDE_API void
balde_session_save(balde_request_t *request, balde_response_t *response)
{
    if (request->priv->session == NULL)
        return;

    gchar *serialized_signed = NULL;
    if (request->priv->session->storage != NULL) {
        gchar *serialized = balde_session_serialize(request->priv->session->storage);
        serialized_signed = balde_session_sign(
            (const guchar*) request->priv->session->key,
            strlen(request->priv->session->key), serialized);
        g_free(serialized);
    }

    const gchar *path;
    if (request->script_name == NULL || request->script_name[0] == '\0')
        path = "/";
    else
        path = request->script_name;

    gchar *domain = NULL;
    if (request->server_name != NULL) {
        gchar **pieces = g_strsplit(request->server_name, ":", 2);
        if (g_strcmp0(pieces[0], "localhost") != 0) {
            if (g_strcmp0(path, "/") == 0)
                domain = g_strdup_printf(".%s", pieces[0]);
            else
                domain = g_strdup(pieces[0]);
        }
        g_strfreev(pieces);
    }

    if (serialized_signed == NULL)
        balde_response_delete_cookie(response, "balde_session", path, domain);
    else
        balde_response_set_cookie(response, "balde_session", serialized_signed,
            request->priv->session->max_age, -1, path, domain, request->https, TRUE);

    g_free(domain);
    g_free(serialized_signed);

    if (request->priv->session->storage != NULL)
        g_hash_table_destroy(request->priv->session->storage);

    g_free(request->priv->session->key);
    g_free(request->priv->session);
}


BALDE_API const gchar*
balde_session_get(balde_request_t *request, const gchar *key)
{
    if (request->priv->session == NULL || request->priv->session->storage == NULL)
        return NULL;

    return g_hash_table_lookup(request->priv->session->storage, key);
}


BALDE_API void
balde_session_set(balde_request_t *request, const gchar *key, const gchar *value)
{
    if (request->priv->session == NULL)
        return;

    if (request->priv->session->storage == NULL)
        request->priv->session->storage = g_hash_table_new_full(g_str_hash,
            g_str_equal, g_free, g_free);

    g_hash_table_insert(request->priv->session->storage, g_strdup(key),
        g_strdup(value));
}


BALDE_API void
balde_session_delete(balde_request_t *request, const gchar *key)
{
    if (request->priv->session == NULL || request->priv->session->storage == NULL)
        return;

    g_hash_table_remove(request->priv->session->storage, key);
}
