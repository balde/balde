if BUILD_EXAMPLES

SUBDIRS = \
	templates

noinst_PROGRAMS = \
	hello \
	hello-with-cookies \
	hello-with-form \
	hello-with-template \
	hello-with-static

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
	resources.c \
	resources.h
hello-with-static.$(OBJEXT): resources.c resources.h

AM_CFLAGS = \
	$(all_includes) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/src/include

AM_LDFLAGS = \
	$(GLIB_LIBS) \
	$(top_builddir)/src/libbalde/libbalde.la

CLEANFILES = \
	resources.c \
	resources.h

resources.xml: $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/static $(srcdir)/resources.xml)

%.c: %.xml
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(srcdir)/static --target $@ $<

%.h: %.xml
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --sourcedir $(srcdir)/static --target $@ $<

endif
