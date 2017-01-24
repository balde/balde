/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
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
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <balde.h>

void sig_handler(int sig)
{
    printf("SIG: %d\n", sig);
    exit(sig);
}


balde_response_t*
hello(balde_app_t *app, balde_request_t *request)
{
    return balde_make_response("Hello World! I'm the balde! :D");
}


int
main(int argc, char **argv)
{
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        printf("Failed to add SIGINT signal handler\n");
        exit(-1);
    }
    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
        printf("Failed to add SIGTERM signal handler\n");
        exit(-1);
    }

    balde_app_t *app = balde_app_init();
    balde_app_add_url_rule(app, "hello", "/", BALDE_HTTP_GET, hello);
    balde_app_run(app, argc, argv);
    balde_app_free(app);
    return 0;
}
//! [Hello world]
