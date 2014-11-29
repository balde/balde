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
#include <balde/app.h>
#include <balde/app-private.h>
#include <balde/routing.h>
#include <balde/routing-private.h>


const gboolean
balde_url_match(const gchar *path, const balde_url_rule_match_t *rule,
    GHashTable **matches)
{
    g_return_val_if_fail(rule != NULL, FALSE);
    GError *_error = NULL;
    if (path == NULL || path[0] == '\0')
        path = "/";
    GMatchInfo *info;
    gboolean rv = TRUE;
    if (!g_regex_match(rule->regex, path, 0, &info)) {
        rv = FALSE;
        goto point1;
    }
    *matches = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    for (guint i = 0; rule->args[i] != NULL; i++) {
        gchar *value = g_match_info_fetch_named(info, rule->args[i]);
        g_hash_table_insert(*matches, g_strdup(rule->args[i]), value);
    }
point1:
    g_match_info_free(info);
    return rv;
}


gchar*
balde_dispatch_from_path(GSList *views, const gchar *path, GHashTable **matches)
{
    for (GSList *tmp = views; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_view_t *view = tmp->data;
        if (balde_url_match(path, view->url_rule->match, matches))
            return g_strdup(view->url_rule->endpoint);
    }
    return NULL;
}


const balde_http_method_t
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
    if (g_strcmp0(method, "PATCH") == 0)
        return BALDE_HTTP_PATCH;
    if (g_strcmp0(method, "DELETE") == 0)
        return BALDE_HTTP_DELETE;
    return BALDE_HTTP_NONE;
}


gchar*
balde_list_allowed_methods(const balde_http_method_t method)
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
    if (method & BALDE_HTTP_PATCH)
        methods = g_slist_append(methods, "PATCH");
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


static gboolean
replace_url_rule_variables_cb(const GMatchInfo *info, GString *res, gpointer data)
{
    gchar *converter = g_match_info_fetch(info, 2);
    gchar *name = g_match_info_fetch(info, 3);
    GSList **tmp = (GSList**) data;
    *tmp = g_slist_append(*tmp, (gchar*) g_strdup(name));
    data = tmp;
    gboolean rv = FALSE;
    if (0 == g_strcmp0(converter, ""))  // string, default
        g_string_append_printf(res, "(?P<%s>[^/]+)", name);
    else if (0 == g_strcmp0(converter, "path"))  // path
        g_string_append_printf(res, "(?P<%s>[^/].*?)", name);
    else
        rv = TRUE;
    g_free(name);
    g_free(converter);
    return rv;
}


balde_url_rule_match_t*
balde_parse_url_rule(const gchar *rule, GError **error)
{
    g_return_val_if_fail(rule != NULL, NULL);
    GError *tmp_error = NULL;
    balde_url_rule_match_t *rv = NULL;
    GRegex *regex_variables = g_regex_new("<(([a-z]+):)?([a-zA-Z_][a-zA-Z0-9_]*)>",
        0, 0, &tmp_error);
    if (tmp_error != NULL) {
        g_propagate_error(error, tmp_error);
        goto point1;
    }
    GSList *args = NULL;
    gchar *escaped_rule = g_regex_escape_string(rule, -1);
    gchar *pattern = g_regex_replace_eval(regex_variables, escaped_rule, -1, 0, 0,
        (GRegexEvalCallback) replace_url_rule_variables_cb, &args, &tmp_error);
    if (tmp_error != NULL) {
        g_propagate_error(error, tmp_error);
        goto point2;
    }
    gchar *tmp_pattern = g_strdup_printf("^%s$", pattern);
    GRegex *regex_final = g_regex_new(tmp_pattern, 0, 0, &tmp_error);
    if (tmp_error != NULL) {
        g_propagate_error(error, tmp_error);
        goto point3;
    }
    gchar **rule_pieces_arr = g_regex_split(regex_variables, escaped_rule, 0);
    GSList *rule_pieces = NULL;
    for (guint i = 0; i < g_strv_length(rule_pieces_arr); i += 4)
        rule_pieces = g_slist_append(rule_pieces, g_strdup(rule_pieces_arr[i]));
    g_strfreev(rule_pieces_arr);
    rule_pieces_arr = g_new(gchar*, g_slist_length(rule_pieces) + 1);
    guint i = 0;
    for (GSList *tmp = rule_pieces; tmp != NULL; tmp = g_slist_next(tmp), i++)
        rule_pieces_arr[i] = (gchar*) tmp->data;
    g_slist_free(rule_pieces);
    rule_pieces_arr[i] = NULL;
    rv = g_new(balde_url_rule_match_t, 1);
    rv->regex = regex_final;
    rv->args = g_new(gchar*, g_slist_length(args) + 1);
    rv->pieces = rule_pieces_arr;
    i = 0;
    for (GSList *tmp = args; tmp != NULL; tmp = g_slist_next(tmp), i++)
        rv->args[i] = (gchar*) tmp->data;
    rv->args[i] = NULL;
point3:
    g_free(tmp_pattern);
point2:
    g_free(pattern);
    g_free(escaped_rule);
    g_slist_free(args);
point1:
    g_regex_unref(regex_variables);
    return rv;
}


void
balde_free_url_rule_match(balde_url_rule_match_t *match)
{
    if (match == NULL)
        return;
    g_strfreev(match->pieces);
    g_strfreev(match->args);
    g_regex_unref(match->regex);
    g_free(match);
}
