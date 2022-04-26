/*
 * balde: A microframework for C based on GLib.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

/**
 * \example hello-with-cookies.c
 *
 * An example with HTTP cookies.
 */

#include <balde.h>


balde_response_t*
hello(balde_app_t *app, balde_request_t *request)
{
    balde_response_t *rv = balde_make_response(balde_request_get_cookie(request, "bola"));
    balde_response_set_cookie(rv, "bola", "guda", 60, -1, NULL, NULL, FALSE, FALSE);
    return rv;
}


int
main(int argc, char **argv)
{
    balde_app_t *app = balde_app_init();
    balde_app_add_url_rule(app, "hello", "/", BALDE_HTTP_GET, hello);
    balde_app_run(app, argc, argv);
    balde_app_free(app);
    return 0;
}
