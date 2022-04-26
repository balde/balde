/*
 * balde: A microframework for C based on GLib.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

/**
 * \example hello-with-template.c
 *
 * An example with template. It depends on more files. Take a look at
 * it on the balde source code.
 */

#include <balde.h>
#include "templates/hello.h"


balde_response_t*
hello(balde_app_t *app, balde_request_t *request)
{
    balde_response_t *response = balde_make_response("");
    const gchar *name = balde_request_get_arg(request, "name");
    balde_response_set_tmpl_var(response, "name", name != NULL ? name : "World");
    balde_template_hello(app, request, response);
    return response;
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
