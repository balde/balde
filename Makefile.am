## Autotools settings

ACLOCAL_AMFLAGS = -I m4

AM_DISTCHECK_CONFIGURE_FLAGS = \
	--enable-examples \
	--enable-http \
	--disable-leg \
	--disable-valgrind


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
	doc/10_getting-started.md \
	doc/20_installation.md \
	doc/30_application-development.md \
	doc/40_application-structure.md \
	doc/50_application-cli.md \
	doc/60_application-deployment.md \
	doc/70_api-changes.md \
	src/template/parser-grammar.leg \
	src/quickstart/project.xml \
	src/quickstart/project/.gitignore.tmpl \
	src/quickstart/project/Makefile.am.tmpl \
	src/quickstart/project/autogen.sh.tmpl \
	src/quickstart/project/build-aux/.keep.tmpl \
	src/quickstart/project/configure.ac.tmpl \
	src/quickstart/project/m4/.keep.tmpl \
	src/quickstart/project/src/foo.c.tmpl \
	src/quickstart/project/src/foo.h.tmpl \
	src/quickstart/project/src/main.c.tmpl \
	src/quickstart/project/src/static/resources.xml.tmpl \
	src/quickstart/project/src/static/screen.css.tmpl \
	src/quickstart/project/src/templates/base.html.tmpl \
	src/quickstart/project/tests/check_foo.c.tmpl \
	examples/static-resources.xml \
	examples/static/foo.js \
	examples/static/foo.css \
	examples/static/asd/bola.txt \
	examples/templates/hello.html \
	examples/templates/form.html \
	examples/templates/url_for.html \
	examples/lighttpd.conf \
	tests/resources.xml \
	tests/static/lol.css \
	tests/static/lol.js \
	tests/static/zz.sh \
	tests/templates/base.c \
	tests/templates/base.html \
	tests/templates/base2.c \
	tests/templates/base2.html \
	tests/templates/base3.c \
	tests/templates/base3.html \
	tests/templates/base4.c \
	tests/templates/base4.html \
	tests/templates/base5.c \
	tests/templates/base5.html \
	tests/templates/base6.c \
	tests/templates/base6.html \
	tests/templates/base7.c \
	tests/templates/base7.html \
	tests/templates/base8.c \
	tests/templates/base8.html \
	tests/templates/body.html \
	tests/templates/body2.html \
	tests/templates/foo.c \
	tests/templates/foo.html \
	tests/templates/foo2.c \
	tests/templates/foo2.html \
	tests/templates/foo3.c \
	tests/templates/foo3.html \
	tests/templates/foo4.c \
	tests/templates/foo4.html \
	tests/templates/foo5.c \
	tests/templates/foo5.html \
	tests/templates/foo6.c \
	tests/templates/foo6.html \
	tests/templates/foo7.c \
	tests/templates/foo7.html \
	tests/templates/foo8.c \
	tests/templates/foo8.html \
	tests/templates/foo9.c \
	tests/templates/foo9.html \
	tests/uploads/simple.txt \
	tests/uploads/without-filename.txt \
	tests/uploads/without-filetype.txt \
	tests/uploads/multiple.txt

CLEANFILES = \
	examples/static-resources.c \
	examples/static-resources.h \
	examples/templates/hello.c \
	examples/templates/hello.h \
	examples/templates/form.c \
	examples/templates/form.h \
	examples/templates/url_for.c \
	examples/templates/url_for.h \
	src/quickstart/project.c \
	src/quickstart/project.h \
	tests/resources.c \
	tests/resources.h \
	tests/utils.c

BUILT_SOURCES = \
	.version

include_HEADERS = \
	src/balde.h

noinst_HEADERS = \
	src/balde-private.h \
	src/app.h \
	src/cgi.h \
	src/datetime.h \
	src/exceptions.h \
	src/httpd.h \
	src/multipart.h \
	src/multipart_parser.h \
	src/requests.h \
	src/resources.h \
	src/responses.h \
	src/routing.h \
	src/scgi.h \
	src/sessions.h \
	src/template/template.h \
	src/template/parser.h \
	src/quickstart/quickstart.h \
	src/utils.h \
	tests/utils.h

lib_LTLIBRARIES = \
	libbalde.la

noinst_LTLIBRARIES = \
	libbalde_template.la \
	libbalde_quickstart.la

bin_PROGRAMS = \
	balde-template-gen \
	balde-quickstart

noinst_PROGRAMS =

check_PROGRAMS = \
	tests/check_app \
	tests/check_cgi \
	tests/check_datetime \
	tests/check_exceptions \
	tests/check_httpd \
	tests/check_multipart \
	tests/check_quickstart \
	tests/check_requests \
	tests/check_resources \
	tests/check_responses \
	tests/check_routing \
	tests/check_scgi \
	tests/check_sessions \
	tests/check_stdin \
	tests/check_template \
	tests/check_template_helpers \
	tests/check_utils

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = balde.pc


## Build rules: libbalde

libbalde_la_SOURCES = \
	src/app.c \
	src/cgi.c \
	src/datetime.c \
	src/exceptions.c \
	src/httpd.c \
	src/multipart.c \
	src/multipart_parser.c \
	src/requests.c \
	src/resources.c \
	src/responses.c \
	src/routing.c \
	src/scgi.c \
	src/sessions.c \
	src/template-helpers.c \
	src/utils.c

libbalde_la_CFLAGS = \
	-fvisibility=hidden \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS)

libbalde_la_LIBADD = \
	$(GLIB_LIBS)

libbalde_la_LDFLAGS = \
	-version-info $(LIBBALDE_LT_VERSION_INFO)


## Build rules: balde-template-gen

libbalde_template_la_SOURCES = \
	src/template/template.c \
	src/template/parser.c \
	src/template/parser-grammar.c

libbalde_template_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	-Wno-unused-function \
	-I$(top_srcdir)/src/template

libbalde_template_la_LIBADD = \
	$(GLIB_LIBS)

if USE_LEG
src/template/parser-grammar.c: src/template/parser-grammar.leg
	$(AM_V_GEN)$(LEG) -o $@ $<
else
src/template/parser-grammar.c:
	$(AM_V_GEN)echo "error: peg/leg not found. failed to build template grammar: $@"; exit 1
endif

balde_template_gen_SOURCES = \
	src/template/balde-template-gen.c

balde_template_gen_CFLAGS = \
	$(GLIB_CFLAGS)

balde_template_gen_LDADD = \
	$(GLIB_LIBS) \
	libbalde_template.la


## Build rules: balde-quickstart

libbalde_quickstart_la_SOURCES = \
	src/quickstart/quickstart.c

nodist_libbalde_quickstart_la_SOURCES = \
	src/quickstart/project.c \
	src/quickstart/project.h

src/quickstart/balde_quickstart-balde-quickstart.$(OBJEXT): \
	$(nodist_libbalde_quickstart_la_SOURCES)

src/quickstart/project.c: src/quickstart/project.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(top_srcdir)/src/quickstart/project $(top_srcdir)/src/quickstart/project.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(top_srcdir)/src/quickstart/project --target $@ $<

src/quickstart/project.h: src/quickstart/project.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(top_srcdir)/src/quickstart/project $(top_srcdir)/src/quickstart/project.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(top_srcdir)/src/quickstart/project --target $@ $<

libbalde_quickstart_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/src/quickstart

libbalde_quickstart_la_LIBADD = \
	$(GLIB_LIBS)

balde_quickstart_SOURCES = \
	src/quickstart/balde-quickstart.c

balde_quickstart_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_builddir)/src/quickstart

balde_quickstart_LDADD = \
	$(GLIB_LIBS) \
	libbalde_quickstart.la


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
	-I$(top_srcdir)/src

examples_hello_LDFLAGS = \
	-no-install

examples_hello_LDADD= \
	$(GLIB_LIBS) \
	libbalde.la

examples_hello_with_cookies_SOURCES = \
	examples/hello-with-cookies.c

examples_hello_with_cookies_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/src

examples_hello_with_cookies_LDFLAGS = \
	-no-install

examples_hello_with_cookies_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

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
	-I$(top_srcdir)/src \
	-I$(top_builddir)/examples

examples_hello_with_form_LDFLAGS = \
	-no-install

examples_hello_with_form_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

examples_hello_with_template_SOURCES = \
	examples/hello-with-template.c

nodist_examples_hello_with_template_SOURCES = \
	examples/templates/hello.c \
	examples/templates/hello.h

examples/examples_hello_with_template-hello-with-template.$(OBJEXT): \
	$(nodist_examples_hello_with_template_SOURCES)

examples_hello_with_template_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/examples

examples_hello_with_template_LDFLAGS = \
	-no-install

examples_hello_with_template_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

examples_hello_with_static_SOURCES = \
	examples/hello-with-static.c

nodist_examples_hello_with_static_SOURCES = \
	examples/static-resources.c \
	examples/static-resources.h

examples/examples_hello_with_static-hello-with-static.$(OBJEXT): \
	$(nodist_examples_hello_with_static_SOURCES)

examples_hello_with_static_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/examples

examples_hello_with_static_LDFLAGS = \
	-no-install

examples_hello_with_static_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

examples_hello_with_url_for_SOURCES = \
	examples/hello-with-url_for.c

nodist_examples_hello_with_url_for_SOURCES = \
	examples/templates/url_for.c \
	examples/templates/url_for.h

examples/examples_hello_with_url_for-hello-with-url_for.$(OBJEXT): \
	$(nodist_examples_hello_with_url_for_SOURCES)

examples_hello_with_url_for_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/examples

examples_hello_with_url_for_LDFLAGS = \
	-no-install

examples_hello_with_url_for_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

examples_hello_with_auth_SOURCES = \
	examples/hello-with-auth.c

examples_hello_with_auth_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/src

examples_hello_with_auth_LDFLAGS = \
	-no-install

examples_hello_with_auth_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

endif

examples/static-resources.c: examples/static-resources.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(top_srcdir)/examples/static $(top_srcdir)/examples/static-resources.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(top_srcdir)/examples/static --target $@ $<

examples/static-resources.h: examples/static-resources.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(top_srcdir)/examples/static $(top_srcdir)/examples/static-resources.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(top_srcdir)/examples/static --target $@ $<

examples/templates/%.c: examples/templates/%.html balde-template-gen
	$(AM_V_GEN)./balde-template-gen $< $@

examples/templates/%.h: examples/templates/%.html balde-template-gen
	$(AM_V_GEN)./balde-template-gen $< $@


## Build rules: tests

tests_check_app_SOURCES = \
	tests/check_app.c

tests_check_app_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_app_LDFLAGS = \
	-static \
	-no-install

tests_check_app_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_cgi_SOURCES = \
	tests/check_cgi.c

tests_check_cgi_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_cgi_LDFLAGS = \
	-static \
	-no-install

tests_check_cgi_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_datetime_SOURCES = \
	tests/check_datetime.c

tests_check_datetime_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_datetime_LDFLAGS = \
	-static \
	-no-install

tests_check_datetime_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_exceptions_SOURCES = \
	tests/check_exceptions.c

tests_check_exceptions_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_exceptions_LDFLAGS = \
	-static \
	-no-install

tests_check_exceptions_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_httpd_SOURCES = \
	tests/check_httpd.c \
	tests/mock_httpd.c

tests_check_httpd_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_httpd_LDFLAGS = \
	-static \
	-no-install

tests_check_httpd_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_multipart_SOURCES = \
	tests/check_multipart.c \
	tests/utils.c

tests_check_multipart_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/tests

tests_check_multipart_LDFLAGS = \
	-static \
	-no-install

tests_check_multipart_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests/tests_check_multipart-check_multipart.$(OBJEXT): \
	tests/uploads/simple.txt \
	tests/uploads/without-filename.txt \
	tests/uploads/without-filetype.txt \
	tests/uploads/multiple.txt

tests_check_quickstart_SOURCES = \
	tests/check_quickstart.c

tests_check_quickstart_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_quickstart_LDFLAGS = \
	-static \
	-no-install

tests_check_quickstart_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la \
	libbalde_quickstart.la

tests_check_resources_SOURCES = \
	tests/check_resources.c

nodist_tests_check_resources_SOURCES = \
	tests/resources.c \
	tests/resources.h
tests/tests_check_resources-check_resources.$(OBJEXT): tests/resources.c tests/resources.h

tests_check_resources_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_builddir)/tests

tests_check_resources_LDFLAGS = \
	-static \
	-no-install

tests_check_resources_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_routing_SOURCES = \
	tests/check_routing.c

tests_check_routing_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_routing_LDFLAGS = \
	-static \
	-no-install

tests_check_routing_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_scgi_SOURCES = \
	tests/check_scgi.c

tests_check_scgi_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_scgi_LDFLAGS = \
	-static \
	-no-install

tests_check_scgi_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_sessions_SOURCES = \
	tests/check_sessions.c \
	tests/mock_sessions.c

tests_check_sessions_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_sessions_LDFLAGS = \
	-static \
	-no-install

tests_check_sessions_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_stdin_SOURCES = \
	tests/check_stdin.c \
	tests/mock_stdin.c

tests_check_stdin_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_stdin_LDFLAGS = \
	-static \
	-no-install

tests_check_stdin_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_template_SOURCES = \
	tests/check_template.c \
	tests/utils.c

tests_check_template_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/tests

tests_check_template_LDFLAGS = \
	-static \
	-no-install

tests_check_template_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la \
	libbalde_template.la

tests/tests_check_template-check_template.$(OBJEXT): \
	tests/templates/base.html \
	tests/templates/base2.html \
	tests/templates/base3.html \
	tests/templates/base4.html \
	tests/templates/base5.html \
	tests/templates/base6.html \
	tests/templates/base7.html \
	tests/templates/base8.html \
	tests/templates/body.html \
	tests/templates/body2.html \
	tests/templates/foo.html \
	tests/templates/foo2.html \
	tests/templates/foo3.html \
	tests/templates/foo4.html \
	tests/templates/foo5.html \
	tests/templates/foo6.html \
	tests/templates/foo7.html \
	tests/templates/foo8.html \
	tests/templates/foo9.html

tests_check_template_helpers_SOURCES = \
	tests/check_template_helpers.c

tests_check_template_helpers_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_template_helpers_LDFLAGS = \
	-static \
	-no-install

tests_check_template_helpers_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_utils_SOURCES = \
	tests/check_utils.c \
	tests/mock_utils.c

tests_check_utils_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_utils_LDFLAGS = \
	-static \
	-no-install

tests_check_utils_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_requests_SOURCES = \
	tests/check_requests.c \
	tests/utils.c

tests_check_requests_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/tests

tests_check_requests_LDFLAGS = \
	-static \
	-no-install

tests_check_requests_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

tests_check_responses_SOURCES = \
	tests/check_responses.c \
	tests/mock_responses.c

tests_check_responses_CFLAGS = \
	$(GLIB_CFLAGS)

tests_check_responses_LDFLAGS = \
	-static \
	-no-install

tests_check_responses_LDADD = \
	$(GLIB_LIBS) \
	libbalde.la

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
		LC_ALL=C \
		$(LIBTOOL) \
			--mode=execute \
			$(VALGRIND) \
				--tool=memcheck \
				--leak-resolution=high \
				--leak-check=yes \
				--show-leak-kinds=definite \
				--errors-for-leak-kinds=definite \
				--error-exitcode=33 \
				--num-callers=20 \
				--suppressions=\"$(top_srcdir)/build-aux/glib.supp\" \
		"
endif


# Helpers: Git version

.version:
	echo $(VERSION) > $@-t && mv $@-t $@

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version
	rm -f $(distdir)/tests/utils.c


# Helpers: Cleanup of helper files

clean-local:
	-rm -rf $(top_builddir)/doc/build/
	-rm -rf $(top_builddir)/valgrind-*.xml
	-rm -rf "${top_builddir}/valgrind.output"


# Helpers: Documentation

if BUILD_DOCS
doc: clean-local Doxyfile
	$(DOXYGEN) Doxyfile

docs: doc
endif
