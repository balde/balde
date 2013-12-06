check_PROGRAMS = \
	check_routing

TESTS = \
	$(check_PROGRAMS)

check_routing_SOURCES = \
	check_routing.c

AM_CFLAGS = \
	$(all_includes) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/lib

AM_LDFLAGS = \
	$(GLIB_LIBS) \
	$(top_srcdir)/lib/libbalde.la
