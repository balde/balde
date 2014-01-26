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
balde_resources_list_childrens(GResource *res, const gchar *path, GSList **list,
    GError **error)
{
    g_return_if_fail(res != NULL && *error == NULL);
    gchar **childrens = g_resource_enumerate_children(res, path, 0, error);
    g_return_if_fail(*error == NULL);
    for (guint i = 0; childrens[i] != NULL; i++) {
        gchar *child = g_strconcat(path, childrens[i], NULL);
        if (g_resource_get_info(res, child, 0, NULL, NULL, NULL))
            *list = g_slist_insert_sorted(*list, (gchar*) g_strdup(child),
                (GCompareFunc) g_strcmp0);
        else
            balde_resources_list_childrens(res, child, list, error);
        g_free(child);
    }
    g_strfreev(childrens);
}


gchar**
balde_resources_list_files(balde_app_t *app)
{
    g_return_val_if_fail(app->static_resources != NULL, NULL);
    GSList *list = NULL;
    balde_resources_list_childrens(app->static_resources, "/", &list, &(app->error));
    g_return_val_if_fail(app->error == NULL, NULL);
    guint i = 0;
    gchar **childrens = g_new(gchar*, g_slist_length(list) + 1);
    for (GSList *tmp = list; tmp != NULL; tmp=tmp->next, i++)
        childrens[i] = (gchar*) tmp->data;
    childrens[i] = NULL;
    g_slist_free(list);
    return childrens;
}
