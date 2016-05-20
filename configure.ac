AC_PREREQ([2.69])

AC_INIT([balde], [m4_esyscmd(build-aux/git-version-gen .tarball-version)],
  [https://github.com/balde/balde], [balde], [https://balde.rgm.io])
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

bkp_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -fvisibility=hidden"
AC_MSG_CHECKING([if ${CC} supports -fvisibility=hidden])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([char guda;])], [
  AC_MSG_RESULT([yes])
], [
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([no -fvisibility=hidden support found in ${CC}, balde requires -fvisibility=hidden])
])
CFLAGS="$bkp_CFLAGS"

AC_ARG_ENABLE([examples], AS_HELP_STRING([--enable-examples], [build examples]))
AS_IF([test "x$enable_examples" = "xyes"], [
  build_examples=yes
  EXAMPLES="enabled"
], [
  build_examples=no
  EXAMPLES="disabled"
])
AM_CONDITIONAL([BUILD_EXAMPLES], [test "x$build_examples" = "xyes"])

AC_ARG_ENABLE([doc], AS_HELP_STRING([--disable-doc],
              [ignore presence of doxygen and disable documentation build]))
AS_IF([test "x$enable_doc" != "xno"], [
  AC_PATH_PROG([doxygen], [doxygen])
  AS_IF([test "x$ac_cv_path_doxygen" = "x"], [
    have_doxygen=no
  ], [
    have_doxygen=yes
  ])
])
AS_IF([test "x$have_doxygen" = "xyes"], , [
  AS_IF([test "x$enable_doc" = "xyes"], [
    AC_MSG_ERROR([doxygen requested but not found])
  ])
])

AM_CONDITIONAL([BUILD_DOCS], [test "x$have_doxygen" = "xyes"])

DOXYGEN="$ac_cv_path_doxygen"

AC_SUBST(DOXYGEN)

AC_ARG_ENABLE([valgrind], AS_HELP_STRING([--disable-valgrind],
              [ignore presence of valgrind]))
AS_IF([test "x$enable_valgrind" != "xno"], [
  AC_PATH_PROG([valgrind], [valgrind])
  AS_IF([test "x$ac_cv_path_valgrind" = "x"], [
    have_valgrind=no
  ], [
    have_valgrind=yes
  ])
])
AS_IF([test "x$have_valgrind" = "xyes"], , [
  AS_IF([test "x$enable_valgrind" = "xyes"], [
    AC_MSG_ERROR([valgrind requested but not found])
  ])
])
AM_CONDITIONAL([USE_VALGRIND], [test "x$have_valgrind" = "xyes"])
VALGRIND="$ac_cv_path_valgrind"
AC_SUBST(VALGRIND)

AC_ARG_ENABLE([leg], AS_HELP_STRING([--disable-leg],
              [ignore presence of peg/leg and disable template parser grammar regeneration]))
AS_IF([test "x$enable_leg" != "xno"], [
  AC_PATH_PROG([leg], [leg])
  AS_IF([test "x$ac_cv_path_leg" = "x"], [
    have_leg=no
  ], [
    have_leg=yes
  ])
])
AS_IF([test "x$have_leg" = "xyes"], , [
  AS_IF([test "x$enable_leg" = "xyes"], [
    AC_MSG_ERROR([peg/leg requested but not found])
  ])
])
AM_CONDITIONAL([USE_LEG], [test "x$have_leg" = "xyes"])
LEG="$ac_cv_path_leg"
AC_SUBST(LEG)

# we need to make sure that shared-mime-info is installed, because we need it
# for accurate mime-type guess
GLIB_DEP="glib-2.0 >= 2.35, gio-2.0 >= 2.35, shared-mime-info"
PC_DEPS="${GLIB_DEP}"
AC_SUBST(PC_DEPS)

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([GLIB], [$GLIB_DEP])

GLIB_COMPILE_RESOURCES="`$PKG_CONFIG --variable glib_compile_resources gio-2.0`"
AC_SUBST(GLIB_COMPILE_RESOURCES)

AC_CHECK_HEADERS([sys/types.h sys/stat.h])

AC_CONFIG_FILES([
    Makefile
    balde.pc
    Doxyfile
    tests/utils.c
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
