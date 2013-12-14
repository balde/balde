check_PROGRAMS = \
	check_app \
	check_exceptions \
	check_routing \
	check_wrappers

TESTS = \
	$(check_PROGRAMS)

check_app_SOURCES = \
	check_app.c

check_exceptions_SOURCES = \
	check_exceptions.c

check_routing_SOURCES = \
	check_routing.c

check_wrappers_SOURCES = \
	check_wrappers.c

AM_CFLAGS = \
	$(all_includes) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

AM_LDFLAGS = \
	$(GLIB_LIBS) \
	$(top_builddir)/lib/libbalde.la
