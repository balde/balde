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

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif /* HAVE_SYS_STAT_H */

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#include <glib.h>
#include <gio/gio.h>
#include "quickstart.h"


gchar*
balde_quickstart_get_app_name(const gchar *dir)
{
    if (dir == NULL)
        return g_strdup("unnamed");
    gchar *dirname = g_path_get_basename(dir);
    if (g_strcmp0(dirname, ".") == 0 || g_strcmp0(dirname, "/") == 0) {
        g_free(dirname);
        return g_strdup("unnamed");
    }
    return dirname;
}


gchar*
balde_quickstart_fix_app_name(const gchar *app_name, gchar replace)
{
    if (app_name == NULL)
        return NULL;
    gchar *app_name_ = g_strdup(app_name);
    for (guint i = 0; app_name_[i] != '\0'; i++) {
        if (g_ascii_isalpha(app_name_[i])) {
            app_name_[i] = g_ascii_tolower(app_name_[i]);
        }
        else {
            app_name_[i] = replace;
        }
    }
    return app_name_;
}


static gchar*
fix_filename(const gchar *filename)
{
    gchar **pieces = g_strsplit(filename, "/", 0);
    gchar *new_filename = g_build_filenamev(pieces);
    g_strfreev(pieces);
    return new_filename;
}


static void
list_project_files(GResource *resource, const gchar *path, GSList **files)
{
    GRegex *re_filename = g_regex_new("^/(.*)\\.tmpl$", 0, 0, NULL);
    gchar **resources = g_resource_enumerate_children(resource, path, 0, NULL);
    gconstpointer data;
    GBytes *b;
    gsize size;
    gchar *filename;
    for (guint i = 0; resources[i] != NULL; i++) {
        gchar *filepath = g_strconcat(path, resources[i], NULL);
        if (g_resource_get_info(resource, filepath, 0, NULL, NULL, NULL)) {
            GMatchInfo *match;
            if (!g_regex_match(re_filename, filepath, 0, &match)) {
                g_match_info_free(match);
                goto clean;
            }
            balde_quickstart_file_t *f = g_new(balde_quickstart_file_t, 1);
            filename = g_match_info_fetch(match, 1);
            g_match_info_free(match);
            f->name = fix_filename(filename);
            g_free(filename);
            f->executable = FALSE;
            if (g_strcmp0(f->name, "autogen.sh") == 0)  // FIXME
                f->executable = TRUE;
            b = g_resources_lookup_data(filepath, 0, NULL);
            data = g_bytes_get_data(b, &size);
            f->content = g_string_new_len((const gchar*) data, size);
            g_bytes_unref(b);
            *files = g_slist_append(*files, f);
        }
        else
            list_project_files(resource, filepath, files);
clean:
        g_free(filepath);
    }
    g_strfreev(resources);
    g_regex_unref(re_filename);
}


GSList*
balde_quickstart_list_project_files(GResource *resource)
{
    GSList *files = NULL;
    list_project_files(resource, "/", &files);
    return files;
}


static void
free_file(balde_quickstart_file_t *f)
{
    g_free(f->name);
    g_string_free(f->content, TRUE);
    g_free(f);
}


void
balde_quickstart_free_files(GSList *l)
{
    g_slist_free_full(l, (GDestroyNotify) free_file);
}


gchar**
balde_quickstart_check_files(GSList *files, const gchar *dir)
{
    GSList *l = NULL;
    gchar **rv = NULL;
    for (GSList *tmp = files; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_quickstart_file_t *f = tmp->data;
        gchar *filename = g_build_filename(dir, f->name, NULL);
        if (g_file_test(filename, G_FILE_TEST_EXISTS))
            l = g_slist_append(l, filename);
        else
            g_free(filename);
    }
    if (l != NULL) {
        rv = g_new(gchar*, g_slist_length(l) + 1);
        guint i = 0;
        for (GSList *tmp = l; tmp != NULL; tmp = g_slist_next(tmp), i++) {
            rv[i] = (gchar*) tmp->data;
        }
        rv[i] = NULL;
    }
    g_slist_free(l);
    return rv;
}


static mode_t
get_mode(void)
{
#if defined(HAVE_SYS_STAT_H) && defined(HAVE_SYS_TYPES_H)
    mode_t m = umask(0);
    umask(m);
    return (S_IRWXU | S_IRWXG | S_IRWXO) & ~m;
#else
    return 0777;
#endif
}


#if defined(HAVE_SYS_STAT_H) && defined(HAVE_SYS_TYPES_H)
static mode_t
get_file_mode(gboolean executable)
{
    mode_t mask = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (executable)
        mask |= (S_IXUSR | S_IXGRP | S_IXOTH);
    return mask & get_mode();
}
#endif


static void
replace_tmpl_var(GString **s, const gchar *name, const gchar *value)
{
    gchar *regex = g_strdup_printf("\\{\\{%s\\}\\}", name);
    GRegex *re = g_regex_new(regex, 0, 0, NULL);
    gchar *replaced = g_regex_replace_literal(re, (*s)->str, (*s)->len, 0,
        value, 0, NULL);
    g_string_free(*s, TRUE);
    *s = g_string_new(replaced);
    g_free(replaced);
    g_regex_unref(re);
    g_free(regex);
}


void
balde_quickstart_write_project(GSList *files, const gchar *dir,
    const gchar *app_name, const gchar *app_version)
{
    gchar *dirname;
    gchar *filename;
    gchar *tmp2;
    for (GSList *tmp = files; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_quickstart_file_t *f = tmp->data;
        replace_tmpl_var(&(f->content), "APP_NAME", app_name);
        replace_tmpl_var(&(f->content), "APP_VERSION", app_version);
        tmp2 = balde_quickstart_fix_app_name(app_name, '-');
        replace_tmpl_var(&(f->content), "APP_BINARY_NAME", tmp2);
        g_free(tmp2);
        tmp2 = balde_quickstart_fix_app_name(app_name, '_');
        replace_tmpl_var(&(f->content), "APP_SAFE_NAME", tmp2);
        g_free(tmp2);
        replace_tmpl_var(&(f->content), "VERSION", PACKAGE_VERSION);
        filename = g_build_filename(dir, f->name, NULL);
        dirname = g_path_get_dirname(filename);
        if (g_mkdir_with_parents(dirname, get_mode()))
            g_printerr("Failed to create directory: %s\n", dirname);
        if (!g_file_set_contents(filename, f->content->str, f->content->len, NULL))
            g_printerr("Failed to create file: %s\n", filename);
#if defined(HAVE_SYS_STAT_H) && defined(HAVE_SYS_TYPES_H)
        chmod(filename, get_file_mode(f->executable));
#endif
        g_free(dirname);
        g_free(filename);
    }
}
