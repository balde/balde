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

#include <string.h>
#include <glib.h>
#include <gio/gio.h>
#include "balde.h"
#include "balde-private.h"
#include "app.h"
#include "datetime.h"
#include "resources.h"
#include "requests.h"
#include "responses.h"


static void
balde_resources_list_childrens(GResource *resources, const gchar *path,
    GSList **list, GError **error)
{
    g_return_if_fail(resources != NULL);
    g_return_if_fail(error == NULL || *error == NULL);
    GError *tmp_error = NULL;
    gchar **childrens = g_resource_enumerate_children(resources, path, 0,
        &tmp_error);
    if (tmp_error != NULL) {
        g_propagate_error(error, tmp_error);
        return;
    }
    for (guint i = 0; childrens[i] != NULL; i++) {
        gchar *child = g_strconcat(path, childrens[i], NULL);
        if (g_resource_get_info(resources, child, 0, NULL, NULL, NULL))
            *list = g_slist_insert_sorted(*list, (gchar*) g_strdup(child),
                (GCompareFunc) g_strcmp0);
        else
            balde_resources_list_childrens(resources, child, list, error);
        g_free(child);
    }
    g_strfreev(childrens);
}


gchar**
balde_resources_list_files(GResource *resources, GError **error)
{
    g_return_val_if_fail(resources != NULL, NULL);
    g_return_val_if_fail(error == NULL || *error == NULL, NULL);
    GSList *list = NULL;
    GError *tmp_error = NULL;
    balde_resources_list_childrens(resources, "/", &list, &tmp_error);
    if (tmp_error != NULL) {
        g_propagate_error(error, tmp_error);
        return NULL;
    }
    gchar **childrens = g_new(gchar*, g_slist_length(list) + 1);
    guint i = 0;
    for (GSList *tmp = list; tmp != NULL; tmp=tmp->next, i++)
        childrens[i] = (gchar*) tmp->data;
    childrens[i] = NULL;
    g_slist_free(list);
    return childrens;
}


void
balde_resource_free(balde_resource_t *resource)
{
    g_return_if_fail(resource != NULL);
    g_free(resource->name);
    g_string_free(resource->content, TRUE);
    g_free(resource->type);
    g_free(resource->hash_name);
    g_free(resource->hash_content);
    g_free(resource);
}


G_LOCK_DEFINE_STATIC(resources);

BALDE_API void
balde_resources_load(balde_app_t *app, GResource *resources)
{
    BALDE_APP_READ_ONLY(app);
    g_return_if_fail(app->error == NULL);
    GError *tmp_error = NULL;
    gchar **resources_list = balde_resources_list_files(resources, &tmp_error);
    if (tmp_error != NULL) {
        g_propagate_error(&(app->error), tmp_error);
        return;
    }
    GBytes *b;
    gsize size;
    gconstpointer data;
    for (guint i = 0; resources_list[i] != NULL; i++) {
        b = g_resource_lookup_data(resources, resources_list[i], 0, &tmp_error);
        if (tmp_error != NULL) {
            g_propagate_error(&(app->error), tmp_error);
            return;
        }
        data = g_bytes_get_data(b, &size);
        balde_resource_t *resource = g_new(balde_resource_t, 1);
        resource->name = g_strdup(resources_list[i]);
        resource->content = g_string_new_len((const gchar*) data, size);
        resource->type = g_content_type_guess(resources_list[i], (const guchar*) data,
            size, NULL);
        resource->hash_name = g_compute_checksum_for_string(G_CHECKSUM_MD5,
            resources_list[i], strlen(resources_list[i]));
        resource->hash_content = g_compute_checksum_for_bytes(G_CHECKSUM_MD5, b);
        G_LOCK(resources);
        app->priv->static_resources = g_slist_append(app->priv->static_resources, resource);
        G_UNLOCK(resources);
        g_bytes_unref(b);
    }
    g_strfreev(resources_list);
}


balde_response_t*
balde_make_response_from_static_resource(balde_app_t *app, balde_request_t *request,
    const gchar *name)
{
    if (app->priv->static_resources == NULL)
        return balde_abort(app, 404);
    for (GSList *tmp = app->priv->static_resources; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_resource_t *resource = tmp->data;
        if (0 == g_strcmp0(name, resource->name)) {
            balde_response_t *response = balde_make_response("");

            gint64 cache_timeout = 60 * 60 * 12;
            gchar *cache_control = g_strdup_printf("public, max-age=%" G_GINT64_FORMAT,
                cache_timeout);
            balde_response_set_header(response, "Cache-Control", cache_control);
            g_free(cache_control);

            GDateTime *now = g_date_time_new_now_utc();
            GDateTime *expires_dt = g_date_time_add(now, G_TIME_SPAN_SECOND * cache_timeout);
            g_date_time_unref(now);
            gchar *expires = balde_datetime_rfc5322(expires_dt);
            g_date_time_unref(expires_dt);
            balde_response_set_header(response, "Expires", expires);
            g_free(expires);

            gchar *etag = g_strdup_printf("\"balde-%s-%s\"", resource->hash_name,
                resource->hash_content);
            balde_response_set_header(response, "Etag", etag);
            const gchar *if_none_match = balde_request_get_header(request,
                "If-None-Match");
            if (if_none_match != NULL && (g_strcmp0(if_none_match, etag) == 0))
                response->status_code = 304;
            else
                balde_response_append_body_len(response, resource->content->str,
                    resource->content->len);
            g_free(etag);
            if (resource->type != NULL)
                balde_response_set_header(response, "Content-Type", resource->type);
            return response;
        }
    }
    return balde_abort(app, 404);
}


balde_response_t*
balde_resource_view(balde_app_t *app, balde_request_t *request)
{
    const gchar* p = balde_request_get_view_arg(request, "file");
    gchar *tmp = g_strdup_printf("/static/%s", p);
    balde_response_t *rv = balde_make_response_from_static_resource(app, request, tmp);
    g_free(tmp);
    return rv;
}
