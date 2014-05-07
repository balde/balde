if BUILD_EXAMPLES

SUBDIRS = \
	static \
	templates

EXTRA_DIST = \
	static-resources.xml

noinst_PROGRAMS = \
	hello \
	hello-with-cookies \
	hello-with-form \
	hello-with-template \
	hello-with-static \
	hello-with-url_for

hello_SOURCES = \
	hello.c

hello_with_cookies_SOURCES = \
	hello-with-cookies.c

hello_with_form_SOURCES = \
	hello-with-form.c \
	templates/hello.c \
	templates/form.c

hello_with_template_SOURCES = \
	hello-with-template.c \
	templates/hello.c

hello_with_static_SOURCES = \
	hello-with-static.c

nodist_hello_with_static_SOURCES = \
	static-resources.c \
	static-resources.h
hello-with-static.$(OBJEXT): static-resources.c static-resources.h

hello_with_url_for_SOURCES = \
	hello-with-url_for.c \
	templates/url_for.c

AM_CFLAGS = \
	$(all_includes) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

AM_LDFLAGS = \
	$(GLIB_LIBS) \
	$(top_builddir)/libbalde/libbalde.la

CLEANFILES = \
	static-resources.c \
	static-resources.h

%.c: %.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/static $(srcdir)/static-resources.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(srcdir)/static --target $@ $<

%.h: %.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/static $(srcdir)/static-resources.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(srcdir)/static --target $@ $<

endif
