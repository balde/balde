noinst_LTLIBRARIES = libbalde_template.la

libbalde_template_la_SOURCES = \
	template.c \
	parser.c \
	parser-grammar.c

libbalde_template_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

libbalde_template_la_LIBADD = \
	$(GLIB_LIBS)

EXTRA_DIST = \
	parser-grammar.leg

if USE_LEG
parser-grammar.c: parser-grammar.leg
	$(AM_V_GEN)$(LEG) -o $@ $<
endif
