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
    balde_app_run(app);
    balde_app_free(app);
    return 0;
}
