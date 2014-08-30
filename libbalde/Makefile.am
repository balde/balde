lib_LTLIBRARIES = libbalde.la

libbalde_la_SOURCES = \
	app.c \
	cgi.c \
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
