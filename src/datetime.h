/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifndef _BALDE_DATETIME_PRIVATE_H
#define _BALDE_DATETIME_PRIVATE_H

#include <glib.h>

gchar* balde_datetime_rfc6265(GDateTime *dt);
gchar* balde_datetime_rfc5322(GDateTime *dt);
gchar* balde_datetime_logging(GDateTime *dt);

#endif /* _BALDE_DATETIME_PRIVATE_H */
