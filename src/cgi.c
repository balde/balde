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
#include <stdio.h>
#include "balde.h"
#include "app.h"
#include "cgi.h"


guint64
balde_cgi_parse_content_length(const gchar *str)
{
    if (str == NULL || str[0] == '\0')
        return 0;
    return g_ascii_strtoull(str, NULL, 10);
}


GString*
balde_cgi_stdin_read(balde_app_t *app)
{
    guint64 clen = balde_cgi_parse_content_length(g_getenv("CONTENT_LENGTH"));
    if (clen == 0)
        return NULL;
    GString *rv = g_string_new("");
    int ch;
    for (guint i = 0; i < clen; i++) {
        if ((ch = getc(stdin)) < 0) {
            balde_abort_set_error(app, 400);
            g_string_free(rv, TRUE);
            return NULL;
        }
        g_string_append_c(rv, (gchar) ch);
    }
    return rv;
}


GHashTable*
balde_cgi_request_headers(void)
{
    gchar **headers = g_listenv();
    GHashTable *rv = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
        g_free);
    for (guint i = 0; headers[i] != NULL; i++) {
        gchar *key = balde_parse_header_name_from_envvar(headers[i]);
        if (key != NULL)
            g_hash_table_insert(rv, key, g_strdup(g_getenv(headers[i])));
    }
    g_strfreev(headers);
    return rv;
}


balde_request_env_t*
balde_cgi_parse_request(balde_app_t *app)
{
    balde_request_env_t *rv = g_new(balde_request_env_t, 1);
    rv->server_name = g_strdup(g_getenv("SERVER_NAME"));
    rv->script_name = g_strdup(g_getenv("SCRIPT_NAME"));
    rv->path_info = g_strdup(g_getenv("PATH_INFO"));
    rv->request_method = g_strdup(g_getenv("REQUEST_METHOD"));
    rv->query_string = g_strdup(g_getenv("QUERY_STRING"));
    rv->headers = balde_cgi_request_headers();
    rv->body = balde_cgi_stdin_read(app);
    rv->https = g_getenv("HTTPS") != NULL;
    return rv;
}


void
balde_cgi_response_print(GString *response)
{
    fwrite(response->str, sizeof(gchar), response->len/sizeof(gchar), stdout);
    g_string_free(response, TRUE);
}


void
balde_cgi_run(balde_app_t *app)
{
    GString *response = balde_app_main_loop(app, NULL,
        balde_response_render, NULL);
    balde_cgi_response_print(response);
}
