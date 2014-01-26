if BUILD_EXAMPLES

SUBDIRS = \
	static \
	templates

noinst_PROGRAMS = \
	hello \
	hello-with-form \
	hello-with-template \
	hello-with-static

hello_SOURCES = \
	hello.c

hello_with_form_SOURCES = \
	hello-with-form.c \
	templates/hello.c \
	templates/form.c

hello_with_template_SOURCES = \
	hello-with-template.c \
	templates/hello.c

hello_with_static_SOURCES = \
	hello-with-static.c \
	static/static.c

AM_CFLAGS = \
	$(all_includes) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

AM_LDFLAGS = \
	$(GLIB_LIBS) \
	$(top_builddir)/lib/libbalde.la

endif
