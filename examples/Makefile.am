if BUILD_EXAMPLES

noinst_PROGRAMS = \
	hello

hello_SOURCES = \
	hello.c

AM_CFLAGS = \
	$(all_includes) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

AM_LDFLAGS = \
	$(GLIB_LIBS) \
	$(top_builddir)/lib/libbalde.la

endif
