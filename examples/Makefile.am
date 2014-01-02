if BUILD_EXAMPLES

SUBDIRS = \
	templates

noinst_PROGRAMS = \
	hello \
	hello-with-form \
	hello-with-template

hello_SOURCES = \
	hello.c

hello_with_form_SOURCES = \
	hello-with-form.c \
	templates/foo.c \
	templates/form.c

hello_with_template_SOURCES = \
	hello-with-template.c \
	templates/foo.c

AM_CFLAGS = \
	$(all_includes) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

AM_LDFLAGS = \
	$(GLIB_LIBS) \
	$(top_builddir)/lib/libbalde.la

endif
