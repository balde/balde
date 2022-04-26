/*
 * balde: A microframework for C based on GLib.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <string.h>
#include "../src/balde.h"
#include "../src/requests.h"
#include "../src/sapi/cgi.h"

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
    GString *body = balde_sapi_cgi_stdin_read(app);
    g_assert_cmpstr(body->str, ==, "guda=bola&moises=arcoiro");
    g_assert_cmpint(body->len, ==, 24);
    g_string_free(body, TRUE);
    balde_app_free(app);
}


void
test_body(void) {
    set_headers();
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, balde_sapi_cgi_parse_request(app));
    g_assert_cmpstr(request->priv->body->str, ==, "guda=bola&moises=arcoiro");
    g_assert_cmpint(request->priv->body->len, ==, 24);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_form(void) {
    set_headers();
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, balde_sapi_cgi_parse_request(app));
    g_assert(request->priv->form != NULL);
    g_assert(g_hash_table_size(request->priv->form) == 2);
    g_assert_cmpstr(g_hash_table_lookup(request->priv->form, "guda"), ==, "bola");
    g_assert_cmpstr(g_hash_table_lookup(request->priv->form, "moises"), ==, "arcoiro");
    balde_request_free(request);
    balde_app_free(app);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/sapi/cgi/stdin/read", test_read);
    g_test_add_func("/sapi/cgi/stdin/body", test_body);
    g_test_add_func("/sapi/cgi/stdin/form", test_form);
    return g_test_run();
}
