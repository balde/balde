check_PROGRAMS = \
	check_app \
	check_exceptions \
	check_routing \
	check_stdin \
	check_wrappers

TESTS = \
	$(check_PROGRAMS)

mock_getc.o: mock_getc.c ../lib/cgi.c

check_app_SOURCES = \
	check_app.c

check_exceptions_SOURCES = \
	check_exceptions.c

check_routing_SOURCES = \
	check_routing.c

check_stdin_SOURCES = \
	check_stdin.c \
	mock_getc.c

check_wrappers_SOURCES = \
	check_wrappers.c

AM_CFLAGS = \
	$(all_includes) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

AM_LDFLAGS = \
	$(GLIB_LIBS) \
	$(top_builddir)/lib/libbalde.la

if USE_VALGRIND
valgrind:
	$(MAKE) check TESTS_ENVIRONMENT=" \
		G_SLICE=always-malloc \
		G_DEBUG=gc-friendly \
		$(LIBTOOL) \
			--mode=execute \
			$(VALGRIND) \
				--tool=memcheck \
				--leak-check=full \
				--leak-resolution=high \
				--num-callers=20"
endif
