/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

/**
 * \example hello-with-static.c
 *
 * An example with static files.  It depends on more files. Take a look at
 * it on the balde source code.
 */

#include <balde.h>
#include "static-resources.h"

// no view required, just hit /static/foo.js, /static/foo.css and /static/asd/bola.txt :)

int
main(int argc, char **argv)
{
    balde_app_t *app = balde_app_init();
    balde_resources_load(app, static_resources_get_resource());
    balde_app_run(app, argc, argv);
    balde_app_free(app);
    return 0;
}
