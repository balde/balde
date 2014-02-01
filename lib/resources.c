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
#include <balde/resources-private.h>


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
