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
#include <gio/gio.h>
#include <balde/app.h>
#include <balde/exceptions.h>
#include <balde/resources-private.h>
#include <balde/wrappers-private.h>


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
    g_free(resource);
}


/**
 * \example hello-with-static.c
 *
 * An example with static files.  It depends on more files. Take a look at
 * it on the balde source code.
 */

void
balde_resources_load(balde_app_t *app, GResource *resources)
{
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
        app->static_resources = g_slist_append(app->static_resources, resource);
        g_bytes_unref(b);
    }
    g_strfreev(resources_list);
}


static balde_response_t*
balde_make_response_from_static_resource(balde_app_t *app, const gchar *name)
{
    if (app->static_resources == NULL)
        return NULL;
    for (GSList *tmp = app->static_resources; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_resource_t *resource = tmp->data;
        if (0 == g_strcmp0(name, resource->name)) {
            GString *tmp = g_string_new_len(resource->content->str, resource->content->len);
            balde_response_t *response = balde_make_response_from_gstring(tmp);
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
    balde_response_t *rv = balde_make_response_from_static_resource(app, tmp);
    g_free(tmp);
    return rv;
}
