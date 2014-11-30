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


gchar*
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
    return (gchar*) g_string_free(rv, FALSE);
}


balde_request_env_t*
balde_cgi_parse_request(balde_app_t *app)
{
    balde_request_env_t *rv = g_new(balde_request_env_t, 1);
    rv->path_info = g_strdup(g_getenv("PATH_INFO"));
    rv->request_method = g_strdup(g_getenv("REQUEST_METHOD"));
    rv->query_string = g_strdup(g_getenv("QUERY_STRING"));
    rv->headers = balde_request_headers();
    rv->body = balde_cgi_stdin_read(app);
    rv->content_length = balde_cgi_parse_content_length(g_getenv("CONTENT_LENGTH"));
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
