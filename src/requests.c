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
#include "cgi.h"
#include "multipart.h"
#include "routing.h"
#include "requests.h"


gchar*
balde_parse_header_name_from_envvar(const gchar *env_name)
{
    if (!g_str_has_prefix(env_name, "HTTP_"))
        return NULL;
    gchar *key = g_new(gchar, strlen(env_name) - 4);
    guint i;
    for (i = 0; env_name[i+5] != '\0'; i++) {
        key[i] = env_name[i+5];
        if (key[i] == '_')
            key[i] = '-';
        key[i] = g_ascii_tolower(key[i]);
    }
    key[i] = '\0';
    return key;
}


gchar*
balde_urldecode(const gchar* str)
{
    // corner case: + -> ' '
    GRegex *re_space = g_regex_new("\\+", 0, 0, NULL);
    gchar *new_str = g_regex_replace_literal(re_space, str, -1, 0, "%20", 0, NULL);
    g_regex_unref(re_space);
    gchar *rv = g_uri_unescape_string(new_str, NULL);
    g_free(new_str);
    return rv;
}


GHashTable*
balde_parse_query_string(const gchar *query_string)
{
    GHashTable *qs = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    if (query_string == NULL)
        goto point1;
    gchar **kv = g_strsplit(query_string, "&", 0);
    for (guint i = 0; kv[i] != NULL; i++) {
        gchar **pieces = g_strsplit(kv[i], "=", 2);
        if (g_strv_length(pieces) != 2)
            goto point2;
        gchar *key = balde_urldecode(pieces[0]);
        gchar *value = balde_urldecode(pieces[1]);
        if (key == NULL || value == NULL) {
            g_free(key);
            g_free(value);
            goto point2;
        }
        g_hash_table_replace(qs, key, value);
point2:
        g_strfreev(pieces);
    }
    g_strfreev(kv);
point1:
    return qs;
}


GHashTable*
balde_parse_cookies(const gchar *cookie_header)
{
    GHashTable *c = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    if (cookie_header == NULL)
        goto point1;
    gchar **kv = g_strsplit(cookie_header, ";", 0);
    for (guint i = 0; kv[i] != NULL; i++) {
        gchar **pieces = g_strsplit(kv[i], "=", 2);
        if (g_strv_length(pieces) != 2)
            goto point2;
        gchar *value = g_strstrip(pieces[1]);
        guint len_value = strlen(value);
        if (len_value > 1 && value[0] == '"' && value[len_value - 1] == '"') {
            value[len_value - 1] = '\0';
            value++;
        }
        g_hash_table_replace(c, g_strdup(g_strstrip(pieces[0])), g_strdup(value));
point2:
        g_strfreev(pieces);
    }
    g_strfreev(kv);
point1:
    return c;
}


balde_authorization_t*
balde_parse_authorization(const gchar *authorization)
{
    if (authorization == NULL)
        return NULL;
    balde_authorization_t *rv = NULL;
    gchar **p = g_strsplit(authorization, " ", 2);
    if (g_strv_length(p) != 2)
        goto point1;
    gchar *type = g_ascii_strdown(p[0], -1);
    if (g_strcmp0(type, "basic") == 0) {
        gsize len;
        guchar *raw = g_base64_decode(p[1], &len);
        gchar *safe_raw = g_strndup((const gchar*) raw, len);
        g_free(raw);
        gchar **b = g_strsplit(safe_raw, ":", 2);
        g_free(safe_raw);
        if (g_strv_length(b) != 2) {
            g_strfreev(b);
            goto point2;
        }
        rv = g_new(balde_authorization_t, 1);
        rv->username = g_strdup(b[0]);
        rv->password = g_strdup(b[1]);
        g_strfreev(b);
    }
    /*
     * FIXME: implement HTTP digest support.
    else if (g_strcmp0(type, "digest") == 0) {
        //
    }
    */
    else {
        g_free(rv);
        rv = NULL;
    }
point2:
    g_free(type);
point1:
    g_strfreev(p);
    return rv;
}


void
balde_authorization_free(balde_authorization_t *authorization)
{
    if (authorization == NULL)
        return;
    g_free((gchar*) authorization->username);
    g_free((gchar*) authorization->password);
    g_free(authorization);
}


balde_request_t*
balde_make_request(balde_app_t *app, balde_request_env_t *request_env)
{
    balde_request_t *request = g_new(balde_request_t, 1);
    request->priv = g_new(struct _balde_request_private_t, 1);
    request->priv->view_args = NULL;
    request->priv->body = NULL;
    request->priv->form = NULL;
    request->priv->files = NULL;
    request->priv->session = NULL;
    balde_request_env_t *env = request_env;
    if (request_env == NULL)
        env = balde_cgi_parse_request(app);
    request->path = env->path_info;
    request->server_name = env->server_name;
    request->script_name = env->script_name;
    if (env->path_info == NULL && env->script_name != NULL) {  // dumb webservers :/
        request->path = env->script_name;
        request->script_name = NULL;
    }
    request->method = balde_http_method_str2enum(env->request_method);
    request->https = env->https;
    request->priv->headers = env->headers;
    request->priv->args = balde_parse_query_string(env->query_string);
    request->priv->cookies = balde_parse_cookies(
        balde_request_get_header(request, "cookie"));
    request->authorization = balde_parse_authorization(
        balde_request_get_header(request, "authorization"));
    if (request->method & (BALDE_HTTP_POST | BALDE_HTTP_PUT | BALDE_HTTP_PATCH)) {
        request->priv->body = env->body;
        const gchar *ct = g_hash_table_lookup(request->priv->headers, "content-type");
        if (ct != NULL && g_str_has_prefix(ct, "multipart/form-data;")) {
            gchar *boundary = balde_multipart_parse_boundary(ct);
            balde_multipart_data_t *data = balde_multipart_parse(boundary,
                request->priv->body);
            g_free(boundary);
            if (data != NULL) {
                request->priv->files = data->files;
                request->priv->form = data->form;
            }
            g_free(data);
        }
        else {
            gchar *tmp = NULL;
            if (request->priv->body != NULL)
                tmp = request->priv->body->str;
            request->priv->form = balde_parse_query_string(tmp);
        }
    }
    g_free(env->query_string);
    g_free(env->request_method);
    g_free(env);
    return request;
}


BALDE_API const gchar*
balde_request_get_header(balde_request_t *request, const gchar *name)
{
    gchar *l_name = g_ascii_strdown(name, -1);
    gchar *value = g_hash_table_lookup(request->priv->headers, l_name);
    g_free(l_name);
    return value;
}


BALDE_API const gchar*
balde_request_get_arg(balde_request_t *request, const gchar *name)
{
    return g_hash_table_lookup(request->priv->args, name);
}


BALDE_API const gchar*
balde_request_get_form(balde_request_t *request, const gchar *name)
{
    if (request->priv->form == NULL)
        return NULL;
    return g_hash_table_lookup(request->priv->form, name);
}


BALDE_API const balde_file_t*
balde_request_get_file(balde_request_t *request, const gchar *name)
{
    if (request->priv->files == NULL)
        return NULL;
    return g_hash_table_lookup(request->priv->files, name);
}


BALDE_API const gchar*
balde_request_get_view_arg(balde_request_t *request, const gchar *name)
{
    if (request->priv->view_args == NULL)
        return NULL;
    return g_hash_table_lookup(request->priv->view_args, name);
}


BALDE_API const gchar*
balde_request_get_cookie(balde_request_t *request, const gchar *name)
{
    if (request->priv->cookies == NULL)
        return NULL;
    return g_hash_table_lookup(request->priv->cookies, name);
}


BALDE_API const GString*
balde_request_get_body(balde_request_t *request)
{
    return request->priv->body;
}


void
balde_request_free(balde_request_t *request)
{
    if (request == NULL)
        return;
    g_free((gchar*) request->path);
    g_free((gchar*) request->server_name);
    g_free((gchar*) request->script_name);
    g_hash_table_destroy(request->priv->headers);
    g_hash_table_destroy(request->priv->args);
    if (request->priv->view_args != NULL)
        g_hash_table_destroy(request->priv->view_args);
    if (request->priv->body != NULL)
        g_string_free(request->priv->body, TRUE);
    if (request->priv->form != NULL)
        g_hash_table_destroy(request->priv->form);
    if (request->priv->files != NULL)
        g_hash_table_destroy(request->priv->files);
    if (request->priv->cookies != NULL)
        g_hash_table_destroy(request->priv->cookies);
    balde_authorization_free(request->authorization);
    g_free(request->priv);
    g_free(request);
}


void
balde_request_env_free(balde_request_env_t *request)
{
    g_free(request->server_name);
    g_free(request->script_name);
    g_free(request->path_info);
    g_free(request->request_method);
    g_free(request->query_string);
    g_hash_table_destroy(request->headers);
    if (request->body != NULL)
        g_string_free(request->body, TRUE);
    g_free(request);
}


BALDE_API gchar*
balde_file_save_to_disk(const balde_file_t *file, const gchar *destdir,
    const gchar *name)
{
    const gchar *fname = name != NULL ? name : file->name;
    if (fname == NULL)
        return NULL;
    gchar *fpath = g_build_filename(destdir, fname, NULL);
    if (g_file_set_contents(fpath, file->content->str, file->content->len, NULL))
        return fpath;
    return NULL;
}


void
balde_file_free(balde_file_t *file)
{
    g_return_if_fail(file != NULL);
    g_free((gchar*) file->name);
    g_free((gchar*) file->type);
    g_string_free(file->content, TRUE);
    g_free(file);
}
