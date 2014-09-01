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
#include <balde/cgi-private.h>
#include <balde/exceptions.h>
#include <balde/wrappers-private.h>


void
balde_stderr_handler(const gchar *str)
{
    fprintf(stderr, "%s", (char*) str);
}


balde_request_env_t*
balde_cgi_parse_request(void)
{
    balde_request_env_t *req = g_new(balde_request_env_t, 1);
    req->script_name = NULL;
    req->path_info = NULL;
    req->request_method = NULL;
    req->query_string = NULL;
    req->headers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    req->content_length = 0;
    req->body = NULL;
    return req;
}


gchar*
balde_stdin_read(balde_app_t *app)
{
    const gchar *clen_str = g_getenv("CONTENT_LENGTH");
    if (clen_str != NULL) {
        guint64 clen = g_ascii_strtoull(clen_str, NULL, 10);
        if (clen <= 0) {
            balde_abort_set_error(app, 400);
            return NULL;
        }
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
    return NULL;
}
