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
#include "balde-private.h"
#include "datetime.h"
#include "exceptions.h"
#include "routing.h"
#include "requests.h"
#include "responses.h"
#include "utils.h"


BALDE_API void
balde_response_set_header(balde_response_t *response, const gchar *name,
    const gchar *value)
{
    // http header name is ascii
    gchar *new_name = g_ascii_strdown(name, -1);
    GSList *values = g_hash_table_lookup(response->priv->headers, new_name);
    GSList *tmp = values;
    values = g_slist_append(values, g_strdup(value));
    if (tmp == NULL)
        g_hash_table_insert(response->priv->headers, new_name, values);
    else
        g_free(new_name);
}


BALDE_API void
balde_response_append_body(balde_response_t *response, const gchar *content)
{
    g_string_append(response->priv->body, content);
}


BALDE_API void
balde_response_append_body_len(balde_response_t *response, const gchar *content,
    const gssize len)
{
    g_string_append_len(response->priv->body, content, len);
}


BALDE_API void
balde_response_truncate_body(balde_response_t *response)
{
    g_string_truncate(response->priv->body, 0);
}


void
balde_response_headers_free(gpointer l)
{
    g_slist_free_full(l, g_free);
}


balde_response_t*
balde_make_response_from_gstring(GString *content)
{
    balde_response_t *response = g_new(balde_response_t, 1);
    response->priv = g_new(struct _balde_response_private_t, 1);
    response->status_code = 200;
    response->priv->headers = g_hash_table_new_full(g_str_hash, g_str_equal,
        g_free, balde_response_headers_free);
    response->priv->template_ctx = g_hash_table_new_full(g_str_hash, g_str_equal,
        g_free, g_free);
    response->priv->body = content;
    return response;
}


BALDE_API balde_response_t*
balde_make_response(const gchar *content)
{
    return balde_make_response_from_gstring(g_string_new(content));
}


BALDE_API balde_response_t*
balde_make_response_len(const gchar *content, const gssize len)
{
    return balde_make_response_from_gstring(g_string_new_len(content, len));
}


BALDE_API void
balde_response_set_tmpl_var(balde_response_t *response, const gchar *name,
    const gchar *value)
{
    g_hash_table_replace(response->priv->template_ctx, g_strdup(name), g_strdup(value));
}


BALDE_API const gchar*
balde_response_get_tmpl_var(balde_response_t *response, const gchar *name)
{
    return g_hash_table_lookup(response->priv->template_ctx, name);
}


BALDE_API const gchar*
balde_response_get_tmpl_var_or_empty(balde_response_t *response, const gchar *name)
{
    const gchar *rv = balde_response_get_tmpl_var(response, name);
    if (rv == NULL)
        return "";
    return rv;
}


BALDE_API void
balde_response_set_cookie(balde_response_t *response, const gchar *name,
    const gchar *value, const gint max_age, const gint64 expires,
    const gchar *path, const gchar *domain, const gboolean secure,
    const gboolean http_only)
{
    GSList *pieces = NULL;
    pieces = g_slist_append(pieces, g_strdup_printf("%s=\"%s\"", name, value));
    if (domain != NULL)
        pieces = g_slist_append(pieces, g_strdup_printf("Domain=\"%s\"", domain));
    if (expires >= 0 || max_age >= 0) {
        GDateTime *exp;
        if (expires < 0) {
            GDateTime *now = g_date_time_new_now_utc();
            exp = g_date_time_add_seconds(now, max_age);
            g_date_time_unref(now);
        }
        else {
            exp = g_date_time_new_from_unix_utc(expires);
        }
        gchar *dt = balde_datetime_rfc6265(exp);
        gchar *tmp = g_strdup_printf("Expires=%s", dt);
        g_free(dt);
        g_date_time_unref(exp);
        pieces = g_slist_append(pieces, tmp);
    }
    if (max_age >= 0)
        pieces = g_slist_append(pieces, g_strdup_printf("Max-Age=%d", max_age));
    if (secure)
        pieces = g_slist_append(pieces, g_strdup("Secure"));
    if (http_only)
        pieces = g_slist_append(pieces, g_strdup("HttpOnly"));
    pieces = g_slist_append(pieces, g_strdup_printf("Path=%s",
        (path != NULL) ? path : "/"));
    GString *val = g_string_new("");
    for (GSList *tmp2 = pieces; tmp2 != NULL; tmp2 = g_slist_next(tmp2)) {
        val = g_string_append(val, (gchar*) tmp2->data);
        if (g_slist_next(tmp2) != NULL)
            val = g_string_append(val, "; ");
    }
    g_slist_free_full(pieces, g_free);
    gchar *tmp = g_string_free(val, FALSE);
    balde_response_set_header(response, "Set-Cookie", tmp);
    g_free(tmp);
}


BALDE_API void
balde_response_delete_cookie(balde_response_t *response, const gchar *name,
    const gchar *path, const gchar *domain)
{
    balde_response_set_cookie(response, name, "", 0, 0, path, domain, FALSE, FALSE);
}


void
balde_response_free(balde_response_t *response)
{
    if (response == NULL)
        return;
    g_hash_table_destroy(response->priv->headers);
    g_hash_table_destroy(response->priv->template_ctx);
    g_string_free(response->priv->body, TRUE);
    g_free(response->priv);
    g_free(response);
}


balde_response_t*
balde_make_response_from_exception(GError *error)
{
    if (error == NULL)
        return NULL;
    guint status_code = error->code;
    const gchar *name = balde_exception_get_name_from_code(status_code);
    gchar* new_description;
    if (name == NULL) {
        status_code = 500;
        name = balde_exception_get_name_from_code(status_code);
        const gchar *description = balde_exception_get_description_from_code(status_code);
        new_description = g_strdup_printf("%s\n\n%s", description,
            error->message);
    }
    else
        new_description = g_strdup(error->message);
    gchar *content = g_strdup_printf(
        "%d %s\n\n%s\n", status_code, name, new_description);
    balde_response_t *response = balde_make_response(content);
    g_free(content);
    g_free(new_description);
    response->status_code = status_code;
    balde_response_set_header(response, "Content-Type", "text/plain; charset=utf-8");
    return response;
}


void
balde_fix_header_name(gchar *name)
{
    for (guint i = 0; name[i] != '\0'; i++)
        if (i == 0 || name[i - 1] == '-')
            name[i] = g_ascii_toupper(name[i]);
}


void
balde_header_render(const gchar *key, GSList *value, GString *str)
{
    gchar *new_key = g_strdup(key);
    balde_fix_header_name(new_key);
    for (GSList *tmp = value; tmp != NULL; tmp = g_slist_next(tmp))
        g_string_append_printf(str, "%s: %s\r\n", new_key, (gchar*) tmp->data);
    g_free(new_key);
}


gchar*
balde_response_generate_etag(balde_response_t *response, gboolean weak)
{
    gchar *hash = g_compute_checksum_for_string(G_CHECKSUM_MD5,
        response->priv->body->str, response->priv->body->len);
    gchar *etag = g_strdup_printf("%s\"%s\"", weak == TRUE ? "W/" : "", hash);
    g_free(hash);
    return etag;
}


BALDE_API void
balde_response_set_etag_header(balde_response_t *response, gboolean weak)
{
    if (g_hash_table_lookup(response->priv->headers, "etag") != NULL)
        return;  // do not override previously set etag
    gchar *hash = balde_response_generate_etag(response, weak);
    balde_response_set_header(response, "Etag", hash);
    g_free(hash);
}

BALDE_API void
balde_response_etag_matching(balde_request_t *request,
    balde_response_t *response)
{
    const gchar *sent_etag = balde_request_get_header(request, "if-none-match");
    if (sent_etag == NULL)
        return;
    gboolean weak_etag = g_str_has_prefix(sent_etag, "W/");
    gchar *calculated_etag = balde_response_generate_etag(response, weak_etag);
    if (g_strcmp0(sent_etag, calculated_etag) == 0) {
        balde_response_truncate_body(response);
        // TODO: Should I use the enum for codes?
        response->status_code = 304;
    }
    g_free(calculated_etag);
}


GString*
balde_response_render(balde_response_t *response, const gboolean with_body)
{
    if (response == NULL)
        return NULL;
    GString *str = g_string_new("");
    if (response->status_code != 200) {
        gchar *n = g_ascii_strup(
            balde_exception_get_name_from_code(response->status_code), -1);
        g_string_append_printf(str, "Status: %d %s\r\n", response->status_code, n);
        g_free(n);
    }
    gchar *len = g_strdup_printf("%zu", response->priv->body->len);
    balde_response_set_header(response, "Content-Length", len);
    g_free(len);
    if (g_hash_table_lookup(response->priv->headers, "content-type") == NULL)
        balde_response_set_header(response, "Content-Type", "text/html; charset=utf-8");
    g_hash_table_foreach(response->priv->headers, (GHFunc) balde_header_render, str);
    g_string_append(str, "\r\n");
    if (with_body)
        g_string_append_len(str, response->priv->body->str,
            response->priv->body->len);
    return str;
}
