lib_LTLIBRARIES = libbalde.la

libbalde_la_SOURCES = \
	app.c \
	cgi.c \
	exceptions.c \
	resources.c \
	routing.c \
	wrappers.c

libbalde_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/src/include

libbalde_la_LDFLAGS = \
	$(AM_LDADD) \
	$(GLIB_LIBS) \
	-version-info $(LT_VERSION_INFO)
