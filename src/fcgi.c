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
#include <fcgiapp.h>
#include <string.h>

#if defined(HAVE_SYS_TYPES_H) && defined(HAVE_UNISTD_H)
#include <sys/types.h>
#include <unistd.h>
#endif

#include "balde.h"
#include "app.h"
#include "cgi.h"
#include "fcgi.h"
#include "wrappers.h"


GHashTable*
balde_fcgi_request_headers(const gchar **envp)
{
    GHashTable *rv = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    for (guint i = 0; envp[i] != NULL; i++) {
        gchar **pieces = g_strsplit(envp[i], "=", 2);
        if (g_strv_length(pieces) != 2)
            goto done;
        gchar *key = balde_parse_header_name_from_envvar(pieces[0]);
        if (key != NULL)
            g_hash_table_insert(rv, key, g_strdup(pieces[1]));
done:
        g_strfreev(pieces);
    }
    return rv;
}


gchar*
balde_fcgi_stdin_read(balde_app_t *app, FCGX_Request *request)
{
    guint64 clen = balde_cgi_parse_content_length(
        FCGX_GetParam("CONTENT_LENGTH", request->envp));
    if (clen == 0)
        return NULL;
    GString *rv = g_string_new("");
    int ch;
    for (guint i = 0; i < clen; i++) {
        if ((ch = FCGX_GetChar(request->in)) < 0) {
            balde_abort_set_error(app, 400);
            g_string_free(rv, TRUE);
            return NULL;
        }
        g_string_append_c(rv, (gchar) ch);
    }
    return (gchar*) g_string_free(rv, FALSE);
}


balde_request_env_t*
balde_fcgi_parse_request(balde_app_t *app, FCGX_Request *request)
{
    balde_request_env_t *rv = g_new(balde_request_env_t, 1);
    rv->script_name = g_strdup(FCGX_GetParam("SCRIPT_NAME", request->envp));
    rv->path_info = g_strdup(FCGX_GetParam("PATH_INFO", request->envp));
    rv->request_method = g_strdup(FCGX_GetParam("REQUEST_METHOD", request->envp));
    rv->query_string = g_strdup(FCGX_GetParam("QUERY_STRING", request->envp));
    rv->headers = balde_fcgi_request_headers((const gchar **) request->envp);
    rv->body = balde_fcgi_stdin_read(app, request);
    rv->content_length = (rv->body != NULL) ? strlen(rv->body) : 0;
    return rv;
}


gpointer
balde_fcgi_thread_run(gpointer data, gpointer user_data)
{
    FCGX_Request *request = data;
    balde_app_t *app = user_data;

    balde_request_env_t *env = balde_fcgi_parse_request(app, request);
    GString *response = balde_app_main_loop(app, env, balde_response_render, NULL);
    FCGX_PutStr(response->str, response->len, request->out);
    g_string_free(response, TRUE);

    FCGX_Finish_r(request);
    g_free(request);
    return NULL;
}


void
balde_fcgi_run(balde_app_t *app, const gchar *host, gint16 port,
    gint max_threads, gint backlog, gboolean listen)
{
    FCGX_Init();

    // initialize socket
    gint sock = 0;
    if (listen) {
        const gchar *final_host = host != NULL ? host : "127.0.0.1";
        g_printerr(" * Running FastCGI on %s:%d (threads: %d, backlog: %d)\n",
            final_host, port, max_threads, backlog);
        gchar *bind = g_strdup_printf("%s:%d", final_host, port);
        sock = FCGX_OpenSocket(bind, backlog);
        g_free(bind);
    }

    // initialize thread pool
    GThreadPool *pool = g_thread_pool_new ((GFunc) balde_fcgi_thread_run, app,
        max_threads, TRUE, NULL);

    // accept requests
    for (;;) {
        FCGX_Request *request = g_new(FCGX_Request, 1);
        FCGX_InitRequest(request, sock, FCGI_FAIL_ACCEPT_ON_INTR);
        if (FCGX_Accept_r(request) < 0) {
            g_free(request);
            break;
        }
        g_thread_pool_push(pool, request, NULL);
    }

    g_thread_pool_free(pool, FALSE, TRUE);
    balde_app_free(app);
}
