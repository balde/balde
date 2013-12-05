TESTS = \
	test_routing

check_PROGRAMS = $(TESTS)

test_routing_SOURCES = \
	routing.c

test_routing_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)

test_routing_LDADD = \
	$(AM_LDADD) \
	$(GLIB_LIBS) \
	$(top_srcdir)/lib/libbalde.la


