bin_PROGRAMS = \
	balde-template-gen

balde_template_gen_SOURCES = \
	balde-template-gen.c

AM_CFLAGS = \
	$(all_includes) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

AM_LDFLAGS = \
	$(GLIB_LIBS) \
	$(top_builddir)/libbalde_template/libbalde_template.la
