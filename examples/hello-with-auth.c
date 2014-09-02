#include <balde.h>


balde_response_t*
hello(balde_app_t *app, balde_request_t *request)
{
    balde_response_t *rv;
    if (request->authorization == NULL) {
        rv = balde_abort(app, 401);
        balde_response_set_header(rv, "WWW-Authenticate",
            "Basic realm=\"Password?! :P\"");
        return rv;
    }
    gchar *tmp = g_strdup_printf("Hello %s, your password is: %s",
        request->authorization->username, request->authorization->password);
    rv = balde_make_response(tmp);
    g_free(tmp);
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
