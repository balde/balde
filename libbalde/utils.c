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
#include <balde/utils-private.h>


/*
 * The following functions are provided to work with base64 encoded strings in
 * a URL-safe way.
 */

gchar*
balde_base64_encode(const guchar *data, gsize len)
{
    GRegex *re_plus = g_regex_new("\\+", 0, 0, NULL);
    GRegex *re_slash = g_regex_new("/", 0, 0, NULL);
    gchar *raw = g_base64_encode(data, len);
    gchar *escaped_plus = g_regex_replace_literal(re_plus, raw, -1, 0,
        "-", 0, NULL);
    g_free(raw);
    gchar *escaped_slash = g_regex_replace_literal(re_slash, escaped_plus,
        -1, 0, "_", 0, NULL);
    g_free(escaped_plus);
    g_regex_unref(re_slash);
    g_regex_unref(re_plus);
    return escaped_slash;
}


guchar*
balde_base64_decode(const gchar *text, gsize *out_len)
{
    GRegex *re_dash = g_regex_new("\\-", 0, 0, NULL);
    GRegex *re_underscore = g_regex_new("_", 0, 0, NULL);
    gchar *unescaped_dash = g_regex_replace_literal(re_dash, text, -1, 0, "+",
        0, NULL);
    gchar *unescaped_underscore = g_regex_replace_literal(re_underscore,
        unescaped_dash, -1, 0, "/", 0, NULL);
    g_free(unescaped_dash);
    guchar *raw = g_base64_decode(unescaped_underscore, out_len);
    g_free(unescaped_underscore);
    g_regex_unref(re_underscore);
    g_regex_unref(re_dash);
    return raw;
}


gint64
balde_timestamp(void) {
    GDateTime *now = g_date_time_new_now_utc();
    gint64 now_uts = g_date_time_to_unix(now);
    g_date_time_unref(now);
    return now_uts - BALDE_EPOCH;
}


gchar*
balde_encoded_timestamp(void) {
    gint64 now = balde_timestamp();
    gchar *now_str = g_strdup_printf("%" G_GINT64_FORMAT, now);
    gchar *rv = balde_base64_encode((guchar*) now_str, strlen(now_str));
    g_free(now_str);
    return rv;
}


gboolean
balde_validate_timestamp(const gchar* timestamp, gint64 max_delta)
{
    gint64 now = balde_timestamp();
    gsize len;
    gchar *ts = (gchar*) balde_base64_decode(timestamp, &len);
    gchar *endptr;
    gint64 old = g_ascii_strtoll(ts, &endptr, 10);
    if (strlen(endptr)){
        // something wrong ocurred during convertion!
        g_free(ts);
        return FALSE;
    }
    g_free(ts);
    return now <= (old + max_delta);
}
