/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <balde/app.h>
#include <balde/routing.h>


gboolean
balde_url_match(gchar *path, const gchar *rule, GHashTable **matches)
{
    GError *_error = NULL;
    gboolean match = FALSE;
    if (path == NULL || path[0] == '\0')
        path = "/";
    gchar **path_pieces = g_strsplit(path, "/", 0);
    gchar **rule_pieces = g_strsplit(rule, "/", 0);
    if (g_strv_length(path_pieces) != g_strv_length(rule_pieces))
        goto point1;
    GRegex *re_variables = g_regex_new("<([^>]+)>", 0, 0, &_error);
    if (NULL != _error) {
        g_error_free(_error);
        goto point2;
    }
    GMatchInfo *match_info;
    *matches = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    for (guint i=0; rule_pieces[i] != NULL; i++) {
        g_regex_match(re_variables, rule_pieces[i], 0, &match_info);
        if (!g_match_info_matches(match_info)) {
            if (0 != g_strcmp0(rule_pieces[i], path_pieces[i])) {
                g_hash_table_destroy(*matches);
                *matches = NULL;
                match = FALSE;
                g_match_info_free(match_info);
                goto point2;
            }
        }
        else {
            gchar* key = g_match_info_fetch(match_info, 1);
            g_hash_table_insert(*matches, key, g_strdup(path_pieces[i]));
        }
        match = TRUE;
        g_match_info_free(match_info);
    }
point2:
    g_regex_unref(re_variables);
point1:
    g_strfreev(rule_pieces);
    g_strfreev(path_pieces);
    return match;
}


gchar*
balde_dispatch_from_path(GSList *views, gchar *path, GHashTable **matches)
{
    for (GSList *tmp = views; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_view_t *view = tmp->data;
        if (balde_url_match(path, view->url_rule->rule, matches))
            return g_strdup(view->url_rule->endpoint);
    }
    return NULL;
}


balde_http_method_t
balde_http_method_str2enum(const gchar *method)
{
    if (g_strcmp0(method, "OPTIONS") == 0)
        return BALDE_HTTP_OPTIONS;
    if (g_strcmp0(method, "GET") == 0)
        return BALDE_HTTP_GET;
    if (g_strcmp0(method, "HEAD") == 0)
        return BALDE_HTTP_HEAD;
    if (g_strcmp0(method, "POST") == 0)
        return BALDE_HTTP_POST;
    if (g_strcmp0(method, "PUT") == 0)
        return BALDE_HTTP_PUT;
    if (g_strcmp0(method, "DELETE") == 0)
        return BALDE_HTTP_DELETE;
    return BALDE_HTTP_NONE;
}


gchar*
balde_list_allowed_methods(balde_http_method_t method)
{
    GSList *methods = NULL;
    if (method & BALDE_HTTP_OPTIONS)
        methods = g_slist_append(methods, "OPTIONS");
    if (method & BALDE_HTTP_DELETE)
        methods = g_slist_append(methods, "DELETE");
    if (method & BALDE_HTTP_GET)
        methods = g_slist_append(methods, "GET");
    if (method & BALDE_HTTP_HEAD)
        methods = g_slist_append(methods, "HEAD");
    if (method & BALDE_HTTP_POST)
        methods = g_slist_append(methods, "POST");
    if (method & BALDE_HTTP_PUT)
        methods = g_slist_append(methods, "PUT");
    gchar** methods_array = g_new(gchar*, g_slist_length(methods) + 1);
    guint i = 0;
    for (GSList *tmp = methods; tmp != NULL; i++, tmp = tmp->next)
        methods_array[i] = (gchar*) tmp->data;
    methods_array[i] = NULL;
    g_slist_free(methods);
    gchar* rv = g_strjoinv(", ", methods_array);
    g_free(methods_array);
    return rv;
}
