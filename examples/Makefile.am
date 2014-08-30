if BUILD_EXAMPLES

SUBDIRS = \
	static \
	templates

EXTRA_DIST = \
	static-resources.xml

CLEANFILES = \
	static-resources.c \
	static-resources.h

noinst_PROGRAMS = \
	hello \
	hello-with-cookies \
	hello-with-form \
	hello-with-template \
	hello-with-static \
	hello-with-url_for \
	hello-with-auth

hello_SOURCES = \
	hello.c

hello_LDADD= \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	$(top_builddir)/libbalde/libbalde.la

hello_with_cookies_SOURCES = \
	hello-with-cookies.c

hello_with_cookies_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	$(top_builddir)/libbalde/libbalde.la

hello_with_form_SOURCES = \
	hello-with-form.c \
	templates/hello.c \
	templates/form.c

hello_with_form_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	$(top_builddir)/libbalde/libbalde.la

hello_with_template_SOURCES = \
	hello-with-template.c \
	templates/hello.c

hello_with_template_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	$(top_builddir)/libbalde/libbalde.la

hello_with_static_SOURCES = \
	hello-with-static.c

nodist_hello_with_static_SOURCES = \
	static-resources.c \
	static-resources.h
hello-with-static.$(OBJEXT): static-resources.c static-resources.h

hello_with_static_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	$(top_builddir)/libbalde/libbalde.la

hello_with_url_for_SOURCES = \
	hello-with-url_for.c \
	templates/url_for.c

hello_with_url_for_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	$(top_builddir)/libbalde/libbalde.la

hello_with_auth_SOURCES = \
	hello-with-auth.c

hello_with_auth_LDADD = \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	$(top_builddir)/libbalde/libbalde.la

AM_CFLAGS = \
	$(all_includes) \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

AM_LDFLAGS = \
	-no-install

%.c: %.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/static $(srcdir)/static-resources.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(srcdir)/static --target $@ $<

%.h: %.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/static $(srcdir)/static-resources.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(srcdir)/static --target $@ $<

endif
