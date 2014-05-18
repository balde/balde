AC_PREREQ(2.68)

AC_INIT([balde], m4_esyscmd([build-aux/git-version-gen .tarball-version]),
    [https://github.com/balde/balde], [balde], [http://balde.io])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

# code changed: REVISION++.
# interface changed: CURRENT++, REVISION=0.
# interface changed (backwards compatible): AGE++, else AGE=0.
LIBBALDE_CURRENT=0
LIBBALDE_AGE=0
LIBBALDE_REVISION=0
LIBBALDE_TEMPLATE_CURRENT=0
LIBBALDE_TEMPLATE_AGE=0
LIBBALDE_TEMPLATE_REVISION=0

LIBBALDE_LT_VERSION_INFO="$LIBBALDE_CURRENT:$LIBBALDE_REVISION:$LIBBALDE_AGE"
LIBBALDE_TEMPLATE_LT_VERSION_INFO="$LIBBALDE_TEMPLATE_CURRENT:$LIBBALDE_TEMPLATE_REVISION:$LIBBALDE_TEMPLATE_AGE"

AC_SUBST(LIBBALDE_LT_VERSION_INFO)
AC_SUBST(LIBBALDE_TEMPLATE_LT_VERSION_INFO)

AM_INIT_AUTOMAKE([foreign dist-bzip2 dist-xz subdir-objects])
AC_CONFIG_HEADERS([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_INIT

AC_PROG_CC_C99
AS_IF([test "x$ac_cv_prog_cc_c99" = "xno"], [
  AC_MSG_ERROR([no C99 compiler found, balde requires a C99 compiler.])
])

AC_SEARCH_LIBS([FCGI_Accept], [fcgi], , [
  AC_MSG_ERROR([FCGI_Accept not found. please install the fastcgi developer kit.])
])

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

        examples:     ${EXAMPLES}

        doxygen:      ${DOXYGEN}
        valgrind:     ${VALGRIND}
        leg:          ${LEG}
")
