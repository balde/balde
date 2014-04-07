SUBDIRS = \
	static

EXTRA_DIST = \
	resources.xml

check_PROGRAMS = \
	check_app \
	check_exceptions \
	check_resources \
	check_routing \
	check_stdin \
	check_template \
	check_template_helpers \
	check_wrappers

TESTS = \
	$(check_PROGRAMS)

mock_getc.o: mock_getc.c $(top_builddir)/libbalde/cgi.c

check_app_SOURCES = \
	check_app.c

check_exceptions_SOURCES = \
	check_exceptions.c

check_resources_SOURCES = \
	check_resources.c

nodist_check_resources_SOURCES = \
	resources.c \
	resources.h
check_resources.$(OBJEXT): resources.c resources.h

check_routing_SOURCES = \
	check_routing.c

check_stdin_SOURCES = \
	check_stdin.c \
	mock_getc.c

check_template_SOURCES = \
	check_template.c

check_template_LDADD = \
	$(top_builddir)/libbalde_template/libbalde_template.la

check_template_helpers_SOURCES = \
	check_template_helpers.c

check_wrappers_SOURCES = \
	check_wrappers.c \
	mock_datetime.c

AM_CFLAGS = \
	$(all_includes) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

AM_LDFLAGS = \
	$(GLIB_LIBS) \
	$(top_builddir)/libbalde/libbalde.la

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
				--num-callers=20 \
				--show-possibly-lost=no"
endif

CLEANFILES = \
	resources.c \
	resources.h

resources.xml: $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/static $(srcdir)/resources.xml)

%.c: %.xml
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(srcdir)/static --target $@ $<

%.h: %.xml
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(srcdir)/static --target $@ $<
