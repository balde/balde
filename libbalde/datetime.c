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

#include <glib.h>
#include <string.h>
#include <balde/datetime-private.h>

// these functions are needed because we want locale-independent date formats.


static const gchar* days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};


static const gchar* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static guint  mday[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


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


/*
 * This function is based on src/http/ngx_http_parse_time.c from NGINX project,
 * that is:
 *
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 *
 * See the file COPYING-nginx.
 */
GDateTime*
balde_datetime_parse(const gchar *value)
{
    gchar *p, *end;
    gint month;
    guint day, year, hour, min, sec;
    guint64 time;

    enum {
        no = 0,
        rfc822,   /* Tue, 10 Nov 2002 23:50:13   */
        rfc850,   /* Tuesday, 10-Dec-02 23:50:13 */
        isoc      /* Tue Dec 10 23:50:13 2002    */
    } fmt;

    fmt = 0;
    end = (gchar*) value + strlen(value);
    day = 32;
    year = 2038;

    for (p = (gchar*) value; p < end; p++) {
        if (*p == ',')
            break;

        if (*p == ' ') {
            fmt = isoc;
            break;
        }
    }

    for (p++; p < end; p++)
        if (*p != ' ')
            break;

    if (end - p < 18)
        return NULL;

    if (fmt != isoc) {
        if (*p < '0' || *p > '9' || *(p + 1) < '0' || *(p + 1) > '9') {
            return NULL;
        }

        day = (*p - '0') * 10 + *(p + 1) - '0';
        p += 2;

        if (*p == ' ') {
            if (end - p < 18)
                return NULL;
            fmt = rfc822;
        }
        else if (*p == '-')
            fmt = rfc850;
        else
            return NULL;

        p++;
    }

    switch (*p) {

        case 'J':
            month = *(p + 1) == 'a' ? 0 : *(p + 2) == 'n' ? 5 : 6;
            break;

        case 'F':
            month = 1;
            break;

        case 'M':
            month = *(p + 2) == 'r' ? 2 : 4;
            break;

        case 'A':
            month = *(p + 1) == 'p' ? 3 : 7;
            break;

        case 'S':
            month = 8;
            break;

        case 'O':
            month = 9;
            break;

        case 'N':
            month = 10;
            break;

        case 'D':
            month = 11;
            break;

        default:
            return NULL;
    }

    p += 3;

    if ((fmt == rfc822 && *p != ' ') || (fmt == rfc850 && *p != '-'))
        return NULL;

    p++;

    if (fmt == rfc822) {
        if (*p < '0' || *p > '9' || *(p + 1) < '0' || *(p + 1) > '9'
            || *(p + 2) < '0' || *(p + 2) > '9'
            || *(p + 3) < '0' || *(p + 3) > '9')
            return NULL;

        year = (*p - '0') * 1000 + (*(p + 1) - '0') * 100
               + (*(p + 2) - '0') * 10 + *(p + 3) - '0';
        p += 4;

    }
    else if (fmt == rfc850) {
        if (*p < '0' || *p > '9' || *(p + 1) < '0' || *(p + 1) > '9')
            return NULL;

        year = (*p - '0') * 10 + *(p + 1) - '0';
        year += (year < 70) ? 2000 : 1900;
        p += 2;
    }

    if (fmt == isoc) {
        if (*p == ' ')
            p++;

        if (*p < '0' || *p > '9')
            return NULL;

        day = *p++ - '0';

        if (*p != ' ') {
            if (*p < '0' || *p > '9')
                return NULL;

            day = day * 10 + *p++ - '0';
        }

        if (end - p < 14)
            return NULL;
    }

    if (*p++ != ' ')
        return NULL;

    if (*p < '0' || *p > '9' || *(p + 1) < '0' || *(p + 1) > '9')
        return NULL;

    hour = (*p - '0') * 10 + *(p + 1) - '0';
    p += 2;

    if (*p++ != ':')
        return NULL;

    if (*p < '0' || *p > '9' || *(p + 1) < '0' || *(p + 1) > '9')
        return NULL;

    min = (*p - '0') * 10 + *(p + 1) - '0';
    p += 2;

    if (*p++ != ':')
        return NULL;

    if (*p < '0' || *p > '9' || *(p + 1) < '0' || *(p + 1) > '9')
        return NULL;

    sec = (*p - '0') * 10 + *(p + 1) - '0';

    if (fmt == isoc) {
        p += 2;

        if (*p++ != ' ')
            return NULL;

        if (*p < '0' || *p > '9' || *(p + 1) < '0' || *(p + 1) > '9'
            || *(p + 2) < '0' || *(p + 2) > '9'
            || *(p + 3) < '0' || *(p + 3) > '9')
            return NULL;

        year = (*p - '0') * 1000 + (*(p + 1) - '0') * 100
               + (*(p + 2) - '0') * 10 + *(p + 3) - '0';
    }

    if (hour > 23 || min > 59 || sec > 59)
         return NULL;

    if (day == 29 && month == 1) {
        if ((year & 3) || ((year % 100 == 0) && (year % 400) != 0))
            return NULL;

    }
    else if (day > mday[month])
        return NULL;

    /*
     * shift new year to March 1 and start months from 1 (not 0),
     * it is needed for Gauss' formula
     */
    if (--month <= 0) {
        month += 12;
        year -= 1;
    }

    /* Gauss' formula for Gregorian days since March 1, 1 BC */
    time = (guint64) (

        /* days in years including leap years since March 1, 1 BC */
        365 * year + year / 4 - year / 100 + year / 400

        /* days before the month */
        + 367 * month / 12 - 30

        /* days before the day */
        + day - 1

        /*
         * 719527 days were between March 1, 1 BC and March 1, 1970,
         * 31 and 28 days were in January and February 1970
         */
        - 719527 + 31 + 28) * 86400 + hour * 3600 + min * 60 + sec;

    return g_date_time_new_from_unix_utc(time);
}
