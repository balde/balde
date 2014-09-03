AC_PREREQ([2.69])

AC_INIT([balde], [m4_esyscmd(build-aux/git-version-gen .tarball-version)],
  [https://github.com/balde/balde], [balde], [http://balde.io])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

# code changed: REVISION++.
# interface changed: CURRENT++, REVISION=0.
# interface changed (backwards compatible): AGE++, else AGE=0.
LIBBALDE_CURRENT=0
LIBBALDE_AGE=0
LIBBALDE_REVISION=0

LIBBALDE_LT_VERSION_INFO="$LIBBALDE_CURRENT:$LIBBALDE_REVISION:$LIBBALDE_AGE"
AC_SUBST(LIBBALDE_LT_VERSION_INFO)

AM_INIT_AUTOMAKE([1.13 foreign dist-bzip2 dist-xz subdir-objects serial-tests])
AC_CONFIG_HEADERS([config.h])
AM_SILENT_RULES([yes])
AM_MAINTAINER_MODE([enable])

LT_INIT

AC_PROG_CC_C99
AS_IF([test "x$ac_cv_prog_cc_c99" = "xno"], [
  AC_MSG_ERROR([no C99 compiler found, balde requires a C99 compiler.])
])

AC_ARG_ENABLE([fastcgi], AS_HELP_STRING([--disable-fastcgi], [disable fastcgi support]))
AS_IF([test "x$enable_fastcgi" != "xno"], [
  AC_SEARCH_LIBS([FCGI_Accept], [fcgi], ,
    AC_MSG_ERROR([FCGI_Accept not found. please install the fastcgi developer kit.])
  )
  AC_DEFINE([ENABLE_FASTCGI], [], [fastcgi support enabled])
  FASTCGI="enabled"
], [
  FASTCGI="disabled"
])

AC_ARG_ENABLE([webserver], AS_HELP_STRING([--disable-webserver], [disable embedded webserver]))
AS_IF([test "x$enable_webserver" != "xno"], [
  build_webserver=yes
  AC_DEFINE([BUILD_WEBSERVER], [], [embedded webserver enabled])
  WEBSERVER="enabled"
], [
  build_webserver=no
  WEBSERVER="disabled"
])
AM_CONDITIONAL([BUILD_WEBSERVER], [test "x$build_webserver" = "xyes"])

AC_ARG_WITH([http-parser],
  AS_HELP_STRING([--with-http-parser=@<:@internal/system@:>@],
    [whether to use system http-parser library [default=internal]]))
use_system_http_parser=no
AS_IF([test "x$build_webserver" = "xyes"], [
  AS_IF([test "x$with_http_parser" = "xsystem"], [
    AC_SEARCH_LIBS([http_parser_init], [http_parser], ,
      AC_MSG_ERROR([http_parser_init not found. please install http-parser or retry with --with-http-parser=internal])
    )
    balde_found_http_parser_headers=no
    AC_CHECK_HEADERS([http_parser.h], [balde_found_http_parser_headers=yes; break;])
    AS_IF([test "x$balde_found_http_parser_headers" != "xyes"], [
      AC_MSG_ERROR([Unable to find http-parser header.])
    ])
    AC_CACHE_CHECK([for system http-parser >= 2.3],
      balde_cv_http_parser_version, [
        AC_RUN_IFELSE([
          AC_LANG_SOURCE([[
#include <http_parser.h>

int main(void) {
    if (HTTP_PARSER_VERSION_MAJOR == 2 && HTTP_PARSER_VERSION_MINOR >= 3)
        return 0;
    return 1;
}
          ]])
        ], [
          balde_cv_http_parser_version=yes
        ], [
          balde_cv_http_parser_version=no
          AC_MSG_ERROR([http-parser < 2.3. update it or retry with --with-http-parser=internal])
        ], [
          balde_cv_http_parser_version=yes
        ])
      ]
    )
    use_system_http_parser=yes
    AC_DEFINE([SYSTEM_HTTP_PARSER], [], [build against system http-parser library])
    WEBSERVER="${WEBSERVER} (system http-parser)"
  ], [
    WEBSERVER="${WEBSERVER} (internal http-parser)"
  ])
],)
AM_CONDITIONAL(USE_SYSTEM_HTTP_PARSER, [test "x$use_system_http_parser" = "xyes"])



AC_ARG_ENABLE([examples], AS_HELP_STRING([--enable-examples], [build examples]))
AS_IF([test "x$enable_examples" = "xyes"], [
  build_examples=yes
  EXAMPLES="enabled"
], [
  build_examples=no
  EXAMPLES="disabled"
])
AM_CONDITIONAL([BUILD_EXAMPLES], [test "x$build_examples" = "xyes"])

AC_ARG_WITH([doxygen], AS_HELP_STRING([--without-doxygen],
            [ignore presence of doxygen and disable documentation build]))
AS_IF([test "x$with_doxygen" != "xno"], [
  AC_PATH_PROG([doxygen], [doxygen])
  AS_IF([test "x$ac_cv_path_doxygen" = "x"], [
    have_doxygen=no
  ], [
    have_doxygen=yes
  ])
  AC_PATH_PROG([github_pages_publish], [github-pages-publish])
])
AS_IF([test "x$have_doxygen" = "xyes"], , [
  AS_IF([test "x$with_doxygen" = "xyes"], [
    AC_MSG_ERROR([doxygen requested but not found])
  ])
])

AM_CONDITIONAL([BUILD_DOCS], [test "x$have_doxygen" = "xyes"])
AM_CONDITIONAL([PUBLISH_DOCS], [test "x$ac_cv_path_github_pages_publish" != "x"])

DOXYGEN="$ac_cv_path_doxygen"
GITHUB_PAGES_PUBLISH="$ac_cv_path_github_pages_publish"

AC_SUBST(DOXYGEN)
AC_SUBST(GITHUB_PAGES_PUBLISH)

AC_ARG_WITH([valgrind], AS_HELP_STRING([--without-valgrind],
            [ignore presence of valgrind]))
AS_IF([test "x$with_valgrind" != "xno"], [
  AC_PATH_PROG([valgrind], [valgrind])
  AS_IF([test "x$ac_cv_path_valgrind" = "x"], [
    have_valgrind=no
  ], [
    have_valgrind=yes
  ])
])
AS_IF([test "x$have_valgrind" = "xyes"], , [
  AS_IF([test "x$with_valgrind" = "xyes"], [
    AC_MSG_ERROR([valgrind requested but not found])
  ])
])
AM_CONDITIONAL([USE_VALGRIND], [test "x$have_valgrind" = "xyes"])
VALGRIND="$ac_cv_path_valgrind"
AC_SUBST(VALGRIND)

AC_ARG_WITH([leg], AS_HELP_STRING([--without-leg],
            [ignore presence of peg/leg and disable template parser grammar regeneration]))
AS_IF([test "x$with_leg" != "xno"], [
  AC_PATH_PROG([leg], [leg])
  AS_IF([test "x$ac_cv_path_leg" = "x"], [
    have_leg=no
  ], [
    have_leg=yes
  ])
])
AS_IF([test "x$have_leg" = "xyes"], , [
  AS_IF([test "x$with_leg" = "xyes"], [
    AC_MSG_ERROR([peg/leg requested but not found])
  ])
])
AM_CONDITIONAL([USE_LEG], [test "x$have_leg" = "xyes"])
LEG="$ac_cv_path_leg"
AC_SUBST(LEG)

# we need to make sure that shared-mime-info is installed, because we need it
# for accurate mime-type guess
GLIB_DEP="glib-2.0 >= 2.34, gio-2.0 >= 2.34, shared-mime-info"
AC_SUBST(GLIB_DEP)

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([GLIB], [$GLIB_DEP])

GLIB_COMPILE_RESOURCES="`$PKG_CONFIG --variable glib_compile_resources gio-2.0`"
AC_SUBST(GLIB_COMPILE_RESOURCES)

AC_CONFIG_FILES([
    Makefile
    artwork/Makefile
    doc/Makefile
    examples/Makefile
    examples/static/Makefile
    examples/static/asd/Makefile
    examples/templates/Makefile
    bin/Makefile
    include/Makefile
    include/balde/Makefile
    include/balde-template/Makefile
    libbalde/Makefile
    libbalde_template/Makefile
    tests/Makefile
    tests/static/Makefile
    balde.pc
    Doxyfile
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
        webserver:    ${WEBSERVER}

        doxygen:      ${DOXYGEN}
        valgrind:     ${VALGRIND}
        leg:          ${LEG}
")
