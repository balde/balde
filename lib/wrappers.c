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
#include <string.h>
#include <balde/app.h>
#include <balde/cgi-private.h>
#include <balde/exceptions.h>
#include <balde/exceptions-private.h>
#include <balde/resources-private.h>
#include <balde/routing-private.h>
#include <balde/wrappers.h>
#include <balde/wrappers-private.h>


void
balde_response_set_header(balde_response_t *response, const gchar *name,
    const gchar *value)
{
    // http header name is ascii
    gchar *new_name = g_new(gchar, strlen(name) + 1);
    guint i;
    for (i = 0; name[i] != '\0'; i++) {
        new_name[i] = g_ascii_tolower(name[i]);
    }
    new_name[i] = '\0';
    g_hash_table_replace(response->headers, new_name, g_strdup(value));
}


void
balde_response_append_body(balde_response_t *response, const gchar *content)
{
    g_string_append(response->body, content);
}


balde_response_t*
balde_make_response(const gchar *content)
{
    balde_response_t *response = g_new(balde_response_t, 1);
    response->status_code = 200;
    response->headers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    response->template_ctx = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    balde_response_set_header(response, "Content-Type", "text/html; charset=utf-8");
    response->body = g_string_new(content);
    return response;
}


void
balde_response_set_tmpl_var(balde_response_t *response, const gchar *name,
    const gchar *value)
{
    g_hash_table_replace(response->template_ctx, g_strdup(name), g_strdup(value));
}


const gchar*
balde_response_get_tmpl_var(balde_response_t *response, const gchar *name)
{
    return g_hash_table_lookup(response->template_ctx, name);
}


void
balde_response_free(balde_response_t *response)
{
    if (response == NULL)
        return;
    g_hash_table_destroy(response->headers);
    g_hash_table_destroy(response->template_ctx);
    g_string_free(response->body, TRUE);
    g_free(response);
}


balde_response_t*
balde_make_response_from_exception(GError *error)
{
    if (error == NULL)
        return NULL;
    guint status_code = error->code;
    const gchar *name = balde_exception_get_name_from_code(status_code);
    gchar* new_description;
    if (name == NULL) {
        status_code = 500;
        name = balde_exception_get_name_from_code(status_code);
        const gchar *description = balde_exception_get_description_from_code(status_code);
        new_description = g_strdup_printf("%s\n\n%s", description,
            error->message);
    }
    else
        new_description = g_strdup(error->message);
    gchar *content = g_strdup_printf(
        "%d %s\n\n%s\n", status_code, name, new_description);
    balde_response_t *response = balde_make_response(content);
    g_free(content);
    g_free(new_description);
    response->status_code = status_code;
    balde_response_set_header(response, "Content-Type", "text/plain; charset=utf-8");
    return response;
}


void
balde_fix_header_name(gchar *name)
{
    for (guint i = 0; name[i] != '\0'; i++)
        if (i == 0 || name[i - 1] == '-')
            name[i] = g_ascii_toupper(name[i]);
}


void
balde_header_render(const gchar *key, const gchar *value, GString *str)
{
    gchar *new_key = g_strdup(key);
    balde_fix_header_name(new_key);
    g_string_append_printf(str, "%s: %s\r\n", new_key, value);
    g_free(new_key);
}


gchar*
balde_response_render(balde_response_t *response, const gboolean with_body)
{
    if (response == NULL)
        return NULL;
    GString *str = g_string_new("");
    if (response->status_code != 200)
        g_string_append_printf(str, "Status: %d %s\r\n", response->status_code,
            balde_exception_get_name_from_code(response->status_code));
    gchar *len = g_strdup_printf("%zu", strlen(response->body->str));
    // we shouldn't trust response->body->len
    balde_response_set_header(response, "Content-Length", len);
    g_free(len);
    g_hash_table_foreach(response->headers, (GHFunc) balde_header_render, str);
    g_string_append(str, "\r\n");
    if (with_body)
        g_string_append(str, response->body->str);
    return g_string_free(str, FALSE);
}


void
balde_response_print(balde_response_t *response, const gboolean with_body)
{
    gchar *resp = balde_response_render(response, with_body);
    g_print("%s", resp);
    g_free(resp);
}


GHashTable*
balde_request_headers(void)
{
    gchar **headers = g_listenv();
    GHashTable *rv = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    for (guint i = 0; headers[i] != NULL; i++) {
        if (g_str_has_prefix(headers[i], "HTTP_")) {
            gchar *key = g_new(gchar, strlen(headers[i]) - 4);
            guint j;
            for (j = 0; headers[i][j+5] != '\0'; j++) {
                key[j] = headers[i][j+5];
                if (key[j] == '_')
                    key[j] = '-';
                key[j] = g_ascii_tolower(key[j]);
            }
            key[j] = '\0';
            g_hash_table_insert(rv, key, g_strdup(g_getenv(headers[i])));
        }
    }
    g_strfreev(headers);
    return rv;
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
        g_hash_table_replace(qs, balde_urldecode(pieces[0]), balde_urldecode(pieces[1]));
point2:
        g_strfreev(pieces);
    }
    g_strfreev(kv);
point1:
    return qs;
}


balde_request_t*
balde_make_request(balde_app_t *app)
{
    balde_request_t *request = g_new(balde_request_t, 1);
    const gchar *path = g_getenv("PATH_INFO");
    if (path == NULL)
        path = "/";
    request->path = g_strdup(path);
    request->method = balde_http_method_str2enum(g_getenv("REQUEST_METHOD"));
    request->headers = balde_request_headers();
    request->args = balde_parse_query_string(g_getenv("QUERY_STRING"));
    request->view_args = NULL;
    if (request->method & (BALDE_HTTP_POST | BALDE_HTTP_PUT | BALDE_HTTP_PATCH)) {
        request->stream = balde_stdin_read(app);
        // TODO: do not load form if content-type isn't form
        request->form = balde_parse_query_string(request->stream);
    }
    else {
        request->stream = NULL;
        request->form = NULL;
    }
    return request;
}


const gchar*
balde_request_get_header(balde_request_t *request, const gchar *name)
{
    gchar *l_name = g_strdup(name);
    guint i;
    for (i = 0; name[i] != '\0'; i++)
        l_name[i] = g_ascii_tolower(l_name[i]);
    l_name[i] = '\0';
    gchar *value = g_hash_table_lookup(request->headers, l_name);
    g_free(l_name);
    return value;
}


const gchar*
balde_request_get_arg(balde_request_t *request, const gchar *name)
{
    return g_hash_table_lookup(request->args, name);
}


const gchar*
balde_request_get_form(balde_request_t *request, const gchar *name)
{
    if (request->form == NULL)
        return NULL;
    return g_hash_table_lookup(request->form, name);
}


const gchar*
balde_request_get_view_arg(balde_request_t *request, const gchar *name)
{
    if (request->view_args == NULL)
        return NULL;
    return g_hash_table_lookup(request->view_args, name);
}


void
balde_request_free(balde_request_t *request)
{
    if (request == NULL)
        return;
    g_free(request->path);
    g_hash_table_destroy(request->headers);
    g_hash_table_destroy(request->args);
    if (request->view_args != NULL)
        g_hash_table_destroy(request->view_args);
    if (request->stream != NULL)
        g_free((gchar*) request->stream);
    if (request->form != NULL)
        g_hash_table_destroy(request->form);
    g_free(request);
}
