lib_LTLIBRARIES = libbalde.la

libbalde_la_SOURCES = \
	app.c \
	cgi.c \
	datetime.c \
	exceptions.c \
	resources.c \
	routing.c \
	template-helpers.c \
	wrappers.c

libbalde_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/include

libbalde_la_LIBADD = \
	$(GLIB_LIBS)

libbalde_la_LDFLAGS = \
	-version-info $(LIBBALDE_LT_VERSION_INFO)

if BUILD_WEBSERVER

libbalde_la_SOURCES += \
	httpd.c

if !USE_SYSTEM_HTTP_PARSER

noinst_HEADERS = \
	http-parser/http_parser.h

libbalde_la_SOURCES += \
	http-parser/http_parser.c

endif

endif
