/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */


/*
 * balde-template-gen is a damn simple code generator, that converts an HTML
 * template to C source, that should be compiled and linked to the balde app.
 *
 * This isn't easy to use but is really fast, and this is what matters! :P
 *
 * Usage: $ balde-template-gen template.html template.[ch]
 *
 * This will generate template.c or template.h. You should add both files to
 * the foo_SOURCES variable in your Makefile.am file, and include template.h
 * in your app source.
 *
 * Also, you may want to add a rule to rebuild your template.c and template.h
 * files when you change template.html.
 */

#include <balde-template/template-private.h>


int
main(int argc, char **argv)
{
    return balde_template_main(argc, argv);
}
