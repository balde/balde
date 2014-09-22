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


static void
balde_thread_run(gpointer data, gpointer user_data)
{
    FCGX_Request *request = data;
    balde_app_t *app = user_data;

    // FIXME: implement request handling

    g_printerr("test\n");
}


G_LOCK_DEFINE_STATIC(request);

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

    FCGX_Request request;

    const gchar *final_host = host != NULL ? host : "127.0.0.1";
    gchar *conn = g_strdup_printf("%s:%d", final_host, port);
    int sockfd = FCGX_OpenSocket(conn, 1024);
    g_free(conn);

    FCGX_InitRequest(&request, sockfd, 0);

    for(;;) {

        G_LOCK(request);
        gint rc = FCGX_Accept_r(&request);
        G_UNLOCK(request);

        if (rc < 0)
            break;

        g_thread_pool_push(pool, &request, &error);

        if (error != NULL) {
            g_printerr("Failed to create new thread: %s", error->message);
            g_error_free(error);
            error = NULL;
            break;
        }

        FCGX_Finish_r(&request);
    }

    g_thread_pool_free(pool, TRUE, FALSE);
}
