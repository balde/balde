/**
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


gboolean
balde_url_match(gchar *path, gchar *rule, GHashTable **matches)
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
    if (NULL != _error)
        goto point2;
    GMatchInfo *match_info;
    *matches = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    for (guint i=0; rule_pieces[i] != NULL; i++) {
        g_regex_match(re_variables, rule_pieces[i], 0, &match_info);
        if (!g_match_info_matches(match_info)) {
            if (0 != g_strcmp0(rule_pieces[i], path_pieces[i])) {
                g_hash_table_destroy(*matches);
                match = FALSE;
                goto point3;
            }
        }
        else {
            gchar* key = g_match_info_fetch(match_info, 1);
            g_hash_table_insert(*matches, key, g_strdup(path_pieces[i]));
        }
        match = TRUE;
    }
point3:
    g_match_info_free(match_info);
    g_regex_unref(re_variables);
point2:
    g_strfreev(rule_pieces);
    g_strfreev(path_pieces);
point1:
    return match;
}
