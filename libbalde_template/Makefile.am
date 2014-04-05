lib_LTLIBRARIES = libbalde_template.la

libbalde_template_la_SOURCES = \
	template.c \
	parser.c \
	parser-grammar.c

libbalde_template_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

libbalde_template_la_LDFLAGS = \
	$(AM_LDADD) \
	$(GLIB_LIBS) \
	-version-info $(LIBBALDE_TEMPLATE_LT_VERSION_INFO)

EXTRA_DIST = \
	parser-grammar.leg

if USE_LEG
parser-grammar.c: parser-grammar.leg
	$(AM_V_GEN)$(LEG) -o $@ $<
endif
