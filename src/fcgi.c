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
    rv->path_info = g_strdup(FCGX_GetParam("PATH_INFO", request->envp));
    rv->request_method = g_strdup(FCGX_GetParam("REQUEST_METHOD", request->envp));
    rv->query_string = g_strdup(FCGX_GetParam("QUERY_STRING", request->envp));
    rv->headers = balde_fcgi_request_headers((const gchar **) request->envp);
    rv->body = balde_fcgi_stdin_read(app, request);
    rv->content_length = (rv->body != NULL) ? strlen(rv->body) : 0;
    return rv;
}


gpointer
balde_fcgi_thread_run(gpointer user_data)
{
    balde_app_t *app = user_data;
    FCGX_Request request;
    FCGX_InitRequest(&request, 0, FCGI_FAIL_ACCEPT_ON_INTR);

    for (;;) {
        G_LOCK_DEFINE_STATIC(request);

        G_LOCK(request);
        gint rc = FCGX_Accept_r(&request);
        G_UNLOCK(request);

        if (rc < 0)
            break;

        balde_request_env_t *env = balde_fcgi_parse_request(app, &request);
        GString *response = balde_app_main_loop(app, env, balde_response_render, NULL);
        FCGX_PutStr(response->str, response->len, request.out);
        g_string_free(response, TRUE);

        FCGX_Finish_r(&request);
    }
    return NULL;
}


#if defined(HAVE_SYS_TYPES_H) && defined(HAVE_UNISTD_H)

static void
balde_fcgi_signal_handler(int signum)
{
    FCGX_ShutdownPending();
    if (signum != SIGINT)
        kill(getpid(), SIGINT);  // it is dumb, stupid, but works... :/
}

#endif


void
balde_fcgi_run(balde_app_t *app, gint max_threads)
{
    FCGX_Init();
    if (max_threads > 1) {

#if defined(HAVE_SYS_TYPES_H) && defined(HAVE_UNISTD_H)
        signal(SIGTERM, balde_fcgi_signal_handler);
        signal(SIGUSR1, balde_fcgi_signal_handler);
#endif

        GThread *threads[max_threads-1];
        for (guint i = 1; i < max_threads; i++) {
            gchar *name = g_strdup_printf("balde-%03d", i);
            threads[i] = g_thread_new(name, balde_fcgi_thread_run, app);
            g_free(name);
        }
    }
    balde_fcgi_thread_run(app);
    balde_app_free(app);
}
