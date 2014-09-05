## Autotools settings

ACLOCAL_AMFLAGS = -I m4

AM_DISTCHECK_CONFIGURE_FLAGS = \
	--enable-examples \
	--enable-webserver \
	--with-http-parser=internal \
	--without-leg


## File listings

EXTRA_DIST = \
	autogen.sh \
	.version \
	build-aux/git-version-gen \
	README.md \
	artwork/README.md \
	artwork/balde-logo.ico \
	artwork/balde-logo.png \
	artwork/balde-logo-small.png \
	artwork/balde-logo.svg \
	doc/style.css \
	doc/00_index.md \
	doc/01_concepts.md \
	doc/02_getting-started.md \
	doc/05_application.md \
	doc/10_requests.md \
	doc/15_responses.md \
	doc/20_template-engine.md \
	doc/25-static-resources.md \
	libbalde/http-parser/COPYING \
	libbalde_template/parser-grammar.leg \
	examples/static-resources.xml \
	examples/static/foo.js \
	examples/static/foo.css \
	examples/static/asd/bola.txt \
	examples/templates/hello.html \
	examples/templates/form.html \
	examples/templates/url_for.html \
	tests/resources.xml \
	tests/static/lol.css \
	tests/static/lol.js \
	tests/static/zz.sh

CLEANFILES = \
	examples/static-resources.c \
	examples/static-resources.h \
	examples/templates/hello.c \
	examples/templates/hello.h \
	examples/templates/form.c \
	examples/templates/form.h \
	examples/templates/url_for.c \
	examples/templates/url_for.h \
	tests/resources.c \
	tests/resources.h

BUILT_SOURCES = \
	.version

include_HEADERS = \
	include/balde.h

pkginclude_HEADERS = \
	include/balde/app.h \
	include/balde/exceptions.h \
	include/balde/resources.h \
	include/balde/routing.h \
	include/balde/sessions.h \
	include/balde/template-helpers.h \
	include/balde/wrappers.h

noinst_HEADERS = \
	include/balde/app-private.h \
	include/balde/cgi-private.h \
	include/balde/datetime-private.h \
	include/balde/exceptions-private.h \
	include/balde/httpd-private.h \
	include/balde/resources-private.h \
	include/balde/routing-private.h \
	include/balde/sessions-private.h \
	include/balde/utils-private.h \
	include/balde/wrappers-private.h \
	include/balde-template/template-private.h \
	include/balde-template/parser-private.h

lib_LTLIBRARIES = \
	libbalde/libbalde.la

noinst_LTLIBRARIES = \
	libbalde_template/libbalde_template.la

bin_PROGRAMS = \
	bin/balde-template-gen

noinst_PROGRAMS =

check_PROGRAMS = \
	tests/check_app \
	tests/check_datetime \
	tests/check_exceptions \
	tests/check_resources \
	tests/check_routing \
	tests/check_sessions \
	tests/check_stdin \
	tests/check_template \
	tests/check_template_helpers \
	tests/check_utils \
	tests/check_wrappers

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = balde.pc


## Build rules: libbalde

libbalde_libbalde_la_SOURCES = \
	libbalde/app.c \
	libbalde/cgi.c \
	libbalde/datetime.c \
	libbalde/exceptions.c \
	libbalde/resources.c \
	libbalde/routing.c \
	libbalde/sessions.c \
	libbalde/template-helpers.c \
	libbalde/utils.c \
	libbalde/wrappers.c

libbalde_libbalde_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

libbalde_libbalde_la_LIBADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS)

libbalde_libbalde_la_LDFLAGS = \
	-version-info $(LIBBALDE_LT_VERSION_INFO)

if BUILD_WEBSERVER

libbalde_libbalde_la_SOURCES += \
	libbalde/httpd.c

if !USE_SYSTEM_HTTP_PARSER

noinst_HEADERS += \
	libbalde/http-parser/http_parser.h

libbalde_libbalde_la_SOURCES += \
	libbalde/http-parser/http_parser.c

endif
endif


## Build rules: balde-template-gen

libbalde_template_libbalde_template_la_SOURCES = \
	libbalde_template/template.c \
	libbalde_template/parser.c \
	libbalde_template/parser-grammar.c

libbalde_template_libbalde_template_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

libbalde_template_libbalde_template_la_LIBADD = \
	$(GLIB_LIBS)

if USE_LEG
libbalde_template/parser-grammar.c: libbalde_template/parser-grammar.leg
	$(AM_V_GEN)$(LEG) -o $@ $<
endif

bin_balde_template_gen_SOURCES = \
	bin/balde-template-gen.c

bin_balde_template_gen_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

bin_balde_template_gen_LDADD = \
	$(GLIB_LIBS) \
	libbalde_template/libbalde_template.la


## Build rules: examples

if BUILD_EXAMPLES

noinst_PROGRAMS += \
	examples/hello \
	examples/hello-with-cookies \
	examples/hello-with-form \
	examples/hello-with-template \
	examples/hello-with-static \
	examples/hello-with-url_for \
	examples/hello-with-auth

examples_hello_SOURCES = \
	examples/hello.c

examples_hello_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

examples_hello_LDFLAGS = \
	-no-install

examples_hello_LDADD= \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

examples_hello_with_cookies_SOURCES = \
	examples/hello-with-cookies.c

examples_hello_with_cookies_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

examples_hello_with_cookies_LDFLAGS = \
	-no-install

examples_hello_with_cookies_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

examples_hello_with_form_SOURCES = \
	examples/hello-with-form.c

nodist_examples_hello_with_form_SOURCES = \
	examples/templates/hello.c \
	examples/templates/hello.h \
	examples/templates/form.c \
	examples/templates/form.h

examples/examples_hello_with_form-hello-with-form.$(OBJEXT): \
	$(nodist_examples_hello_with_form_SOURCES)

examples_hello_with_form_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/examples

examples_hello_with_form_LDFLAGS = \
	-no-install

examples_hello_with_form_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

examples_hello_with_template_SOURCES = \
	examples/hello-with-template.c

nodist_examples_hello_with_template_SOURCES = \
	examples/templates/hello.c \
	examples/templates/hello.h

examples/examples_hello_with_template-hello-with-template.$(OBJEXT): \
	$(nodist_examples_hello_with_template_SOURCES)

examples_hello_with_template_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/examples

examples_hello_with_template_LDFLAGS = \
	-no-install

examples_hello_with_template_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

examples_hello_with_static_SOURCES = \
	examples/hello-with-static.c

nodist_examples_hello_with_static_SOURCES = \
	examples/static-resources.c \
	examples/static-resources.h

examples/examples_hello_with_static-hello-with-static.$(OBJEXT): \
	$(nodist_examples_hello_with_static_SOURCES)

examples_hello_with_static_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/examples

examples_hello_with_static_LDFLAGS = \
	-no-install

examples_hello_with_static_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

examples_hello_with_url_for_SOURCES = \
	examples/hello-with-url_for.c

nodist_examples_hello_with_url_for_SOURCES = \
	examples/templates/url_for.c \
	examples/templates/url_for.h

examples/examples_hello_with_url_for-hello-with-url_for.$(OBJEXT): \
	$(nodist_examples_hello_with_url_for_SOURCES)

examples_hello_with_url_for_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/examples

examples_hello_with_url_for_LDFLAGS = \
	-no-install

examples_hello_with_url_for_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

examples_hello_with_auth_SOURCES = \
	examples/hello-with-auth.c

examples_hello_with_auth_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

examples_hello_with_auth_LDFLAGS = \
	-no-install

examples_hello_with_auth_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

endif

examples/%-resources.c: examples/%-resources.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(top_srcdir)/examples/static $(top_srcdir)/examples/static-resources.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(top_srcdir)/examples/static --target $@ $<

examples/%-resources.h: examples/%-resources.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(top_srcdir)/examples/static $(top_srcdir)/examples/static-resources.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(top_srcdir)/examples/static --target $@ $<

examples/templates/%.c: examples/templates/%.html bin/balde-template-gen
	$(AM_V_GEN)bin/balde-template-gen $< $@

examples/templates/%.h: examples/templates/%.html bin/balde-template-gen
	$(AM_V_GEN)bin/balde-template-gen $< $@


## Build rules: tests

if BUILD_WEBSERVER

check_PROGRAMS += \
	tests/check_httpd

tests_check_httpd_SOURCES = \
	tests/check_httpd.c \
	tests/mock_datetime2.c

tests_check_httpd_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

tests_check_httpd_LDFLAGS = \
	-no-install

tests_check_httpd_LDADD = \
	$(GLIB_LIBS) \
	libbalde/libbalde.la

endif

tests_check_app_SOURCES = \
	tests/check_app.c

tests_check_app_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

tests_check_app_LDFLAGS = \
	-no-install

tests_check_app_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

tests_check_datetime_SOURCES = \
	tests/check_datetime.c

tests_check_datetime_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

tests_check_datetime_LDFLAGS = \
	-no-install

tests_check_datetime_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

tests_check_exceptions_SOURCES = \
	tests/check_exceptions.c

tests_check_exceptions_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

tests_check_exceptions_LDFLAGS = \
	-no-install

tests_check_exceptions_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

tests_check_resources_SOURCES = \
	tests/check_resources.c

nodist_tests_check_resources_SOURCES = \
	tests/resources.c \
	tests/resources.h
tests/tests_check_resources-check_resources.$(OBJEXT): tests/resources.c tests/resources.h

tests_check_resources_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/tests

tests_check_resources_LDFLAGS = \
	-no-install

tests_check_resources_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

tests_check_routing_SOURCES = \
	tests/check_routing.c

tests_check_routing_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

tests_check_routing_LDFLAGS = \
	-no-install

tests_check_routing_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

tests_check_sessions_SOURCES = \
	tests/check_sessions.c \
	tests/mock_encoded_timestamp.c

tests_check_sessions_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

tests_check_sessions_LDFLAGS = \
	-no-install

tests_check_sessions_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

tests_check_stdin_SOURCES = \
	tests/check_stdin.c \
	tests/mock_getc.c

tests_check_stdin_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

tests_check_stdin_LDFLAGS = \
	-no-install

tests_check_stdin_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

tests_check_template_SOURCES = \
	tests/check_template.c

tests_check_template_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

tests_check_template_LDFLAGS = \
	-no-install

tests_check_template_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la \
	libbalde_template/libbalde_template.la

tests_check_template_helpers_SOURCES = \
	tests/check_template_helpers.c

tests_check_template_helpers_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

tests_check_template_helpers_LDFLAGS = \
	-no-install

tests_check_template_helpers_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

tests_check_utils_SOURCES = \
	tests/check_utils.c \
	tests/mock_datetime_utils.c

tests_check_utils_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

tests_check_utils_LDFLAGS = \
	-no-install

tests_check_utils_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

tests_check_wrappers_SOURCES = \
	tests/check_wrappers.c \
	tests/mock_datetime.c

tests_check_wrappers_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

tests_check_wrappers_LDFLAGS = \
	-no-install

tests_check_wrappers_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	libbalde/libbalde.la

tests/resources.c: tests/resources.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(top_srcdir)/tests/static $(top_srcdir)/tests/resources.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(top_srcdir)/tests/static --target $@ $<

tests/resources.h: tests/resources.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(top_srcdir)/tests/static $(top_srcdir)/tests/resources.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(top_srcdir)/tests/static --target $@ $<

TESTS = \
	$(check_PROGRAMS)


## Helpers: Valgrind runners

if USE_VALGRIND
valgrind: all
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

valgrind-ci: all clean-local
	$(MAKE) check TESTS_ENVIRONMENT=" \
		G_SLICE=always-malloc \
		G_DEBUG=gc-friendly \
		$(LIBTOOL) \
			--mode=execute \
			$(VALGRIND) \
				--tool=memcheck \
				--xml=yes \
				--xml-file=valgrind-%p.xml \
				--leak-check=full \
				--leak-resolution=high \
				--num-callers=20 \
				--show-possibly-lost=no"
endif


# Helpers: Git version

.version:
	echo $(VERSION) > $@-t && mv $@-t $@

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version


# Helpers: Cleanup of helper files

clean-local:
	-rm -rf doc/build/
	-rm -rf tests/valgrind-*.xml


# Helpers: Documentation

if BUILD_DOCS
doxygen: Doxyfile
	$(MAKE) clean-local
	$(DOXYGEN) Doxyfile

if PUBLISH_DOCS
publish_docs: doxygen
	$(GITHUB_PAGES_PUBLISH) --verbose \
		--cname docs.balde.io \
		. doc/build/html
endif
endif
