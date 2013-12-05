AC_PREREQ(2.68)

AC_INIT([balde], m4_esyscmd([build-aux/git-version-gen .tarball-version]),
    [https://github.com/rafaelmartins/balde/issues], [balde],
    [https://github.com/rafaelmartins/balde])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz subdir-objects])
AC_CONFIG_HEADERS([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_INIT

AC_PROG_CC_C99
AS_IF([test "x$ac_cv_prog_cc_c99" = "xno"], [
  AC_MSG_ERROR([no C99 compiler found, balde requires a C99 compiler.])
])

AC_ARG_ENABLE([fastcgi], AS_HELP_STRING([--enable-fastcgi], [enable fastcgi support]))
AS_IF([test "x$enable_fastcgi" = "xyes"], [
  AC_SEARCH_LIBS([FCGI_Accept], [fcgi], ,
    AC_MSG_ERROR([FCGI_Accept not found. please install the fastcgi developer kit.])
  )
  AC_DEFINE([ENABLE_FASTCGI], [], [fastcgi support enabled])
  FASTCGI="enabled"
], [
  FASTCGI="disabled"
])

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.34])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

AS_ECHO("
        ==== ${PACKAGE_STRING} ====

        prefix:       ${prefix}
        exec_prefix:  ${exec_prefix}
        bindir:       ${bindir}

        compiler:     ${CC}
        cflags:       ${CFLAGS}
        ldflags:      ${LDFLAGS}

        fastcgi:      ${FASTCGI}
")
