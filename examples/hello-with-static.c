#include <balde.h>
#include "resources.h"

// no view required, just hit /static/foo.js, /static/foo.css and /static/asd/bola.txt :)

int
main(int argc, char **argv)
{
    balde_app_t *app = balde_app_init();
    balde_resources_load(app, resources_get_resource());
    balde_app_run(app);
    balde_app_free(app);
    return 0;
}
