AC_PREREQ(2.68)

AC_INIT([balde], m4_esyscmd([build-aux/git-version-gen .tarball-version]),
    [https://github.com/rafaelmartins/balde/issues], [balde],
    [https://github.com/rafaelmartins/balde])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

# code changed: REVISION++.
# interface changed: CURRENT++, REVISION=0.
# interface changed (backwards compatible): AGE++, else AGE=0.
CURRENT=0
AGE=0
REVISION=0

LT_VERSION_INFO="$CURRENT:$REVISION:$AGE"
AC_SUBST(LT_VERSION_INFO)

AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz subdir-objects])
AC_CONFIG_HEADERS([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_INIT

AC_PROG_CC_C99
AS_IF([test "x$ac_cv_prog_cc_c99" = "xno"], [
  AC_MSG_ERROR([no C99 compiler found, balde requires a C99 compiler.])
])

AC_ARG_ENABLE([fastcgi], AS_HELP_STRING([--enable-fastcgi], [enable fastcgi support]), [
  AC_SEARCH_LIBS([FCGI_Accept], [fcgi], ,
    AC_MSG_ERROR([FCGI_Accept not found. please install the fastcgi developer kit.])
  )
  AC_DEFINE([ENABLE_FASTCGI], [], [fastcgi support enabled])
  FASTCGI="enabled"
], [
  FASTCGI="disabled"
])

AC_ARG_ENABLE([examples], AS_HELP_STRING([--enable-examples], [build examples]), [
  build_examples=true
  EXAMPLES="enabled"
], [
  build_examples=false
  EXAMPLES="disabled"
])
AM_CONDITIONAL([BUILD_EXAMPLES], [test "x$build_examples" = "xtrue"])

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.34])

AC_CONFIG_FILES([
    Makefile
    examples/Makefile
    include/Makefile
    include/balde/Makefile
    lib/Makefile
    tests/Makefile
])
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
        examples:     ${EXAMPLES}
")
