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

#include <balde/app.h>
#include <balde/app-private.h>
#include <balde/fcgi-private.h>
#include <balde/wrappers-private.h>


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


/*{
    balde_fcgi_request_t *request = data;
    balde_app_t *app = user_data;

    // FIXME: implement request handling

    g_printerr("test\n");
    FCGX_FPrintF(request->out, "Content-Type: text/html\r\n\r\nbola");

}



void
balde_fcgi_run(balde_app_t *app, const gchar *host, gint16 port, gint max_threads)
{
    FCGX_Init();
    GError *error = NULL;
    GThreadPool *pool = g_thread_pool_new(balde_thread_run, app, max_threads,
        FALSE, &error);

    if (error != NULL) {
        g_printerr("Failed to create thread pool: %s", error->message);
        g_error_free(error);
        return;
    }

    balde_fcgi_request_t *b_request = g_new(balde_fcgi_request_t, 1);
    FCGX_Request request;

    const gchar *final_host = host != NULL ? host : "127.0.0.1";
    gchar *conn = g_strdup_printf("%s:%d", final_host, port);
    int sockfd = FCGX_OpenSocket(conn, 1024);
    g_free(conn);

    FCGX_InitRequest(&request, sockfd, 0);

    for(;;) {

        G_LOCK(request);
        gint rc = FCGX_Accept_r(&request);
        b_request->envp = g_strdupv(request.envp);
        b_request->in = request.in;
        b_request->out = request.out;
        b_request->err = request.err;
        G_UNLOCK(request);

        if (rc < 0)
            break;

        g_thread_pool_push(pool, b_request, &error);

        if (error != NULL) {
            g_printerr("Failed to create new thread: %s", error->message);
            g_error_free(error);
            error = NULL;
            break;
        }

        FCGX_Finish_r(&request);
    }

    g_thread_pool_free(pool, TRUE, FALSE);
}*/



balde_request_env_t*
balde_fcgi_parse_request(FCGX_Request *request)
{
    balde_request_env_t *rv = g_new(balde_request_env_t, 1);
    rv->path_info = FCGX_GetParam("PATH_INFO", request->envp);;
    rv->request_method = FCGX_GetParam("REQUEST_METHOD", request->envp);
    rv->query_string = FCGX_GetParam("QUERY_STRING", request->envp);
    rv->headers = balde_fcgi_request_headers((const gchar **) request->envp);
    rv->content_length = 0;
    rv->body = NULL;
    return rv;
}


gpointer
balde_thread_run(gpointer data)
{
    balde_fcgi_thread_ctx_t *ctx = data;
    FCGX_Request request;
    FCGX_InitRequest(&request, 0, 0);

    for (;;) {
        G_LOCK_DEFINE_STATIC(request);

        G_LOCK(request);
        gint rc = FCGX_Accept_r(&request);
        G_UNLOCK(request);

        if (rc < 0)
            break;

        balde_request_env_t *env = balde_fcgi_parse_request(&request);
        GString *response = balde_app_main_loop(ctx->app, env, balde_response_render, NULL);

        FCGX_FPrintF(request.out, "%s", response->str);

        FCGX_Finish_r(&request);
    }
    return NULL;
}


void
balde_fcgi_run(balde_app_t *app, const gchar *host, gint16 port, gint max_threads)
{
    GThread *threads[max_threads];
    FCGX_Init();
    const gchar *final_host = host != NULL ? host : "127.0.0.1";
    for (guint i = 1; i < max_threads; i++) {
        balde_fcgi_thread_ctx_t *ctx = g_new(balde_fcgi_thread_ctx_t, 1);
        ctx->host = final_host;
        ctx->port = port + i;
        ctx->app = app;
        ctx->id = i;
        gchar *name = g_strdup_printf("balde-%d", ctx->port);
        threads[i] = g_thread_new(name, balde_thread_run, ctx);
        g_free(name);
    }
    balde_fcgi_thread_ctx_t *ctx = g_new(balde_fcgi_thread_ctx_t, 1);
    ctx->host = final_host;
    ctx->port = port;
    ctx->app = app;
    ctx->id = 0;
    balde_thread_run(ctx);
}
