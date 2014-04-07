#include <balde.h>
#include "templates/hello.h"
#include "templates/form.h"

balde_response_t*
hello(balde_app_t *app, balde_request_t *request)
{
    balde_response_t *response = balde_make_response("");
    if (request->method == BALDE_HTTP_POST) {
        const gchar *name = balde_request_get_form(request, "name");
        balde_response_set_tmpl_var(response, "name",
            name != NULL && name[0] != '\0' ? name : "World");
        balde_template_hello(app, request, response);
    }
    else
        balde_template_form(app, request, response);
    return response;
}


int
main(int argc, char **argv)
{
    balde_app_t *app = balde_app_init();
    balde_app_add_url_rule(app, "hello", "/", BALDE_HTTP_GET | BALDE_HTTP_POST,
        hello);
    balde_app_run(app);
    balde_app_free(app);
    return 0;
}
