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
#include "../src/balde.h"
#include "../src/requests.h"
#include "../src/responses.h"


balde_response_t*
arcoiro_view(balde_app_t *app, balde_request_t *req)
{
    return NULL;
}


void
test_tmpl_url_for(void)
{
    g_setenv("PATH_INFO", "/", TRUE);
    g_setenv("REQUEST_METHOD", "GET", TRUE);
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    balde_app_add_url_rule(app, "arcoiro", "/arcoiro/<bola>/<guda>/",
        BALDE_HTTP_GET, arcoiro_view);
    balde_app_add_url_rule(app, "arcoiro2", "/arco/<iro>", BALDE_HTTP_GET,
        arcoiro_view);
    gchar *url = balde_tmpl_url_for(app, request, "arcoiro", FALSE, "chunda", "guto");
    g_assert_cmpstr(url, ==, "/arcoiro/chunda/guto/");
    g_free(url);
    url = balde_tmpl_url_for(app, request, "arcoiro2", FALSE, "bola");
    g_assert_cmpstr(url, ==, "/arco/bola");
    g_free(url);
    url = balde_tmpl_url_for(app, request, "arcoiro2", FALSE, "bo\"la");
    g_assert_cmpstr(url, ==, "/arco/bo%22la");
    g_free(url);
    url = balde_tmpl_url_for(app, request, "static", FALSE, "foo/jquery-min.js");
    g_assert_cmpstr(url, ==, "/static/foo/jquery-min.js");
    g_free(url);
    balde_request_free(request);
    balde_app_free(app);
    g_unsetenv("REQUEST_METHOD");
    g_unsetenv("PATH_INFO");
}


void
test_tmpl_url_for_with_script_name(void)
{
    g_setenv("PATH_INFO", "/", TRUE);
    g_setenv("REQUEST_METHOD", "GET", TRUE);
    g_setenv("SCRIPT_NAME", "/foo/bar", TRUE);
    // FIXME: this thing is too weak :(
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    balde_app_add_url_rule(app, "arcoiro", "/arcoiro/<bola>/<guda>/",
        BALDE_HTTP_GET, arcoiro_view);
    balde_app_add_url_rule(app, "arcoiro2", "/arco/<iro>", BALDE_HTTP_GET,
        arcoiro_view);
    gchar *url = balde_tmpl_url_for(app, request, "arcoiro", FALSE, "chunda", "guto");
    g_assert_cmpstr(url, ==, "/foo/bar/arcoiro/chunda/guto/");
    g_free(url);
    url = balde_tmpl_url_for(app, request, "arcoiro2", FALSE, "bola");
    g_assert_cmpstr(url, ==, "/foo/bar/arco/bola");
    g_free(url);
    url = balde_tmpl_url_for(app, request, "static", FALSE, "foo/jquery-min.js");
    g_assert_cmpstr(url, ==, "/foo/bar/static/foo/jquery-min.js");
    g_free(url);
    balde_request_free(request);
    balde_app_free(app);
    g_unsetenv("SCRIPT_NAME");
    g_unsetenv("REQUEST_METHOD");
    g_unsetenv("PATH_INFO");
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/template_helpers/url_for", test_tmpl_url_for);
    g_test_add_func("/template_helpers/url_for_with_script_name",
        test_tmpl_url_for_with_script_name);
    return g_test_run();
}
