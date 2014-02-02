#include <balde.h>
#include "resources.h"


balde_response_t*
hello(balde_app_t *app, balde_request_t *request)
{
    return balde_make_response_from_static_resource(app, "/static/foo.js");
}


int
main(int argc, char **argv)
{
    balde_app_t *app = balde_app_init();
    balde_resources_load(app, resources_get_resource());
    balde_app_add_url_rule(app, "hello", "/", BALDE_HTTP_GET, hello);
    balde_app_run(app);
    balde_app_free(app);
}
