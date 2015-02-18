/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

/**
 * \example hello.c
 *
 * A hello world!
 */

//! [Hello world]
#include <balde.h>


balde_response_t*
hello(balde_app_t *app, balde_request_t *request)
{
    return balde_make_response("Hello World! I'm the balde! :D");
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
//! [Hello world]
