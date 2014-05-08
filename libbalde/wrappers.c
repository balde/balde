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
    gchar *new_name = g_ascii_strdown(name, -1);
    GSList *values = g_hash_table_lookup(response->headers, new_name);
    GSList *tmp = values;
    values = g_slist_append(values, g_strdup(value));
    if (tmp == NULL)
        g_hash_table_insert(response->headers, new_name, values);
    else
        g_free(new_name);
}


void
balde_response_append_body(balde_response_t *response, const gchar *content)
{
    g_string_append(response->body, content);
}


void
balde_response_append_body_len(balde_response_t *response, const gchar *content,
    const gssize len)
{
    g_string_append_len(response->body, content, len);
}


void
balde_response_headers_free(gpointer l)
{
    g_slist_free_full(l, g_free);
}


balde_response_t*
balde_make_response_from_gstring(GString *content)
{
    balde_response_t *response = g_new(balde_response_t, 1);
    response->status_code = 200;
    response->headers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
        balde_response_headers_free);
    response->template_ctx = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    response->body = content;
    return response;
}


balde_response_t*
balde_make_response(const gchar *content)
{
    return balde_make_response_from_gstring(g_string_new(content));
}


balde_response_t*
balde_make_response_len(const gchar *content, const gssize len)
{
    return balde_make_response_from_gstring(g_string_new_len(content, len));
}


void
balde_response_set_tmpl_var(balde_response_t *response, const gchar *name,
    const gchar *value)
{
    g_hash_table_replace(response->template_ctx, g_strdup(name), g_strdup(value));
}


/**
 * \example hello-with-template.c
 *
 * An example with template. It depends on more files. Take a look at
 * it on the balde source code.
 */

const gchar*
balde_response_get_tmpl_var(balde_response_t *response, const gchar *name)
{
    return g_hash_table_lookup(response->template_ctx, name);
}


static const gchar* days[] = {
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
    "Sun",
};


static const gchar* months[] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
};


void
balde_response_set_cookie(balde_response_t *response, const gchar *name,
    const gchar *value, const gint max_age, const gint64 expires,
    const gchar *path, const gchar *domain, const gboolean secure)
{
    GSList *pieces = NULL;
    pieces = g_slist_append(pieces, g_strdup_printf("%s=\"%s\"", name, value));
    if (domain != NULL)
        pieces = g_slist_append(pieces, g_strdup_printf("Domain=\"%s\"", domain));
    if (expires >= 0 || max_age >= 0) {
        GDateTime *exp;
        if (expires < 0) {
            GDateTime *now = g_date_time_new_now_utc();
            exp = g_date_time_add_seconds(now, max_age);
            g_date_time_unref(now);
        }
        else {
            exp = g_date_time_new_from_unix_utc(expires);
        }
        gchar *tmp = g_strdup_printf("Expires=%s, %02d-%s-%04d %02d:%02d:%02d GMT",
            days[g_date_time_get_day_of_week(exp) - 1],
            g_date_time_get_day_of_month(exp),
            months[g_date_time_get_month(exp) - 1], g_date_time_get_year(exp),
            g_date_time_get_hour(exp), g_date_time_get_minute(exp),
            g_date_time_get_second(exp));
        g_date_time_unref(exp);
        pieces = g_slist_append(pieces, tmp);
    }
    if (max_age >= 0)
        pieces = g_slist_append(pieces, g_strdup_printf("Max-Age=%d", max_age));
    if (secure)
        pieces = g_slist_append(pieces, g_strdup("Secure"));
    pieces = g_slist_append(pieces, g_strdup_printf("Path=%s",
        (path != NULL) ? path : "/"));
    GString *val = g_string_new("");
    for (GSList *tmp = pieces; tmp != NULL; tmp = g_slist_next(tmp)) {
        val = g_string_append(val, (gchar*) tmp->data);
        if (g_slist_next(tmp) != NULL)
            val = g_string_append(val, "; ");
    }
    g_slist_free_full(pieces, g_free);
    gchar *tmp = g_string_free(val, FALSE);
    balde_response_set_header(response, "Set-Cookie", tmp);
    g_free(tmp);
}


void
balde_response_delete_cookie(balde_response_t *response, const gchar *name,
    const gchar *path, const gchar *domain)
{
    balde_response_set_cookie(response, name, "", 0, 0, path, domain, FALSE);
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
balde_header_render(const gchar *key, GSList *value, GString *str)
{
    gchar *new_key = g_strdup(key);
    balde_fix_header_name(new_key);
    for (GSList *tmp = value; tmp != NULL; tmp = g_slist_next(tmp))
        g_string_append_printf(str, "%s: %s\r\n", new_key, (gchar*) tmp->data);
    g_free(new_key);
}


GString*
balde_response_render(balde_response_t *response, const gboolean with_body)
{
    if (response == NULL)
        return NULL;
    GString *str = g_string_new("");
    if (response->status_code != 200)
        g_string_append_printf(str, "Status: %d %s\r\n", response->status_code,
            balde_exception_get_name_from_code(response->status_code));
    gchar *len = g_strdup_printf("%zu", response->body->len);
    balde_response_set_header(response, "Content-Length", len);
    g_free(len);
    if (g_hash_table_lookup(response->headers, "content-type") == NULL)
        balde_response_set_header(response, "Content-Type", "text/html; charset=utf-8");
    g_hash_table_foreach(response->headers, (GHFunc) balde_header_render, str);
    g_string_append(str, "\r\n");
    if (with_body)
        g_string_append_len(str, response->body->str, response->body->len);
    return str;
}


void
balde_response_print(balde_response_t *response, const gboolean with_body)
{
    GString *resp = balde_response_render(response, with_body);
    fwrite(resp->str, sizeof(gchar), resp->len/sizeof(gchar), stdout);
    g_string_free(resp, TRUE);
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
    request->cookies = balde_parse_cookies(g_getenv("HTTP_COOKIE"));
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
    gchar *l_name = g_ascii_strdown(name, -1);
    gchar *value = g_hash_table_lookup(request->headers, l_name);
    g_free(l_name);
    return value;
}


const gchar*
balde_request_get_arg(balde_request_t *request, const gchar *name)
{
    return g_hash_table_lookup(request->args, name);
}


/**
 * \example hello-with-form.c
 *
 * An example with HTTP forms. It depends on more files. Take a look at
 * it on the balde source code.
 */

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


/**
 * \example hello-with-cookies.c
 *
 * An example with HTTP cookies.
 */

const gchar*
balde_request_get_cookie(balde_request_t *request, const gchar *name)
{
    if (request->cookies == NULL)
        return NULL;
    return g_hash_table_lookup(request->cookies, name);
}


void
balde_request_free(balde_request_t *request)
{
    if (request == NULL)
        return;
    g_free((gchar*) request->path);
    g_hash_table_destroy(request->headers);
    g_hash_table_destroy(request->args);
    if (request->view_args != NULL)
        g_hash_table_destroy(request->view_args);
    g_free((gchar*) request->stream);
    if (request->form != NULL)
        g_hash_table_destroy(request->form);
    if (request->cookies != NULL)
        g_hash_table_destroy(request->cookies);
    g_free(request);
}
