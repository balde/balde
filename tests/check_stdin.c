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
#include <balde/wrappers-private.h>

const gchar *query_string = "guda=bola&moises=arcoiro";


void
set_headers(void)
{
    gchar* len = g_strdup_printf("%d", (int) strlen(query_string));
    g_setenv("CONTENT_LENGTH", len, TRUE);
    g_setenv("REQUEST_METHOD", "POST", TRUE);
    g_free(len);
}


void
test_read(void) {
    set_headers();
    balde_app_t *app = balde_app_init();
    gchar *body = balde_stdin_read(app);
    g_assert_cmpstr(body, ==, "guda=bola&moises=arcoiro");
    g_free(body);
    balde_app_free(app);
}


void
test_stream(void) {
    set_headers();
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app);
    g_assert_cmpstr(request->stream, ==, "guda=bola&moises=arcoiro");
    balde_request_free(request);
    balde_app_free(app);
}


void
test_form(void) {
    set_headers();
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app);
    g_assert(request->form != NULL);
    g_assert(g_hash_table_size(request->form) == 2);
    g_assert_cmpstr(g_hash_table_lookup(request->form, "guda"), ==, "bola");
    g_assert_cmpstr(g_hash_table_lookup(request->form, "moises"), ==, "arcoiro");
    balde_request_free(request);
    balde_app_free(app);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/stdin/read", test_read);
    g_test_add_func("/stdin/stream", test_stream);
    g_test_add_func("/stdin/form", test_form);
    return g_test_run();
}
