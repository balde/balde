/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <string.h>
#include "balde.h"
#include "datetime.h"

// these functions are needed because we want locale-independent date formats.


static const gchar* days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};


static const gchar* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


gchar*
balde_datetime_rfc6265(GDateTime *dt)
{
    // datetime must be utc
    return g_strdup_printf("%s, %02d-%s-%04d %02d:%02d:%02d GMT",
        days[g_date_time_get_day_of_week(dt) - 1],
        g_date_time_get_day_of_month(dt), months[g_date_time_get_month(dt) - 1],
        g_date_time_get_year(dt), g_date_time_get_hour(dt),
        g_date_time_get_minute(dt), g_date_time_get_second(dt));
}


gchar*
balde_datetime_rfc5322(GDateTime *dt)
{
    // datetime must be utc
    return g_strdup_printf("%s, %02d %s %04d %02d:%02d:%02d GMT",
        days[g_date_time_get_day_of_week(dt) - 1],
        g_date_time_get_day_of_month(dt), months[g_date_time_get_month(dt) - 1],
        g_date_time_get_year(dt), g_date_time_get_hour(dt),
        g_date_time_get_minute(dt), g_date_time_get_second(dt));
}


gchar*
balde_datetime_logging(GDateTime *dt)
{
    return g_strdup_printf("%02d/%s/%04d %02d:%02d:%02d",
        g_date_time_get_day_of_month(dt), months[g_date_time_get_month(dt) - 1],
        g_date_time_get_year(dt), g_date_time_get_hour(dt),
        g_date_time_get_minute(dt), g_date_time_get_second(dt));
}
