lib_LTLIBRARIES = libbalde.la

libbalde_la_SOURCES = \
	app.c \
	cgi.c \
	exceptions.c \
	httpd.c \
	resources.c \
	routing.c \
	template-helpers.c \
	wrappers.c \
	http-parser/http_parser.c

libbalde_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

libbalde_la_LDFLAGS = \
	$(AM_LDADD) \
	$(GLIB_LIBS) \
	-version-info $(LIBBALDE_LT_VERSION_INFO)
