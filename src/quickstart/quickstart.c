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
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include <glib.h>
#include <gio/gio.h>
#include "quickstart.h"
#include "project.h"


gchar*
balde_quickstart_get_name(const gchar *name)
{
    if (name != NULL)
        return g_strdup(name);

    gchar *cwd = g_get_current_dir();
    gchar *dirname = g_path_get_basename(cwd);
    g_free(cwd);
    if (g_strcmp0(dirname, ".") == 0 || g_strcmp0(dirname, "/") == 0) {
        g_free(dirname);
        return g_strdup("unnamed");
    }
    return dirname;
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
balde_quickstart_list_project_files(void)
{
    GSList *files = NULL;
    list_project_files(project_get_resource(), "/", &files);
    return files;
}


gboolean
balde_quickstart_check_files(GSList *files, const gchar *dir)
{
    for (GSList *tmp = files; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_quickstart_file_t *f = tmp->data;
        gchar *filename = g_build_filename(dir, f->name, NULL);
        gboolean t = g_file_test(filename, G_FILE_TEST_EXISTS);
        g_free(filename);
        if (t)
            return FALSE;
    }
    return TRUE;
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


void
balde_quickstart_write_project(GSList *files, const gchar *dir)
{
    gchar *dirname;
    gchar *filename;
    for (GSList *tmp = files; tmp != NULL; tmp = g_slist_next(tmp)) {
        balde_quickstart_file_t *f = tmp->data;
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
