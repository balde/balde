/*
 * balde: A microframework for C based on GLib.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_TESTS_UTILS_H
#define _BALDE_TESTS_UTILS_H

gchar* get_template(const gchar *filename);
gchar* get_expected_template(const gchar *filename);
GString* get_upload(const gchar *filename);

#endif
