bin_PROGRAMS = \
	balde-template-gen

balde_template_gen_SOURCES = \
	balde-template-gen.c

AM_CFLAGS = \
	$(all_includes) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/src/include

AM_LDFLAGS = \
	$(GLIB_LIBS) \
	$(top_builddir)/src/libbalde/libbalde.la
