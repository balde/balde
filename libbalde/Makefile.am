lib_LTLIBRARIES = libbalde.la

libbalde_la_SOURCES = \
	app.c \
	cgi.c \
	exceptions.c \
	resources.c \
	routing.c \
	sessions.c \
	template-helpers.c \
	utils.c \
	wrappers.c

libbalde_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(JSON_GLIB_CFLAGS) \
	-I$(top_srcdir)/include

libbalde_la_LDFLAGS = \
	$(AM_LDADD) \
	$(GLIB_LIBS) \
	$(JSON_GLIB_LIBS) \
	-version-info $(LIBBALDE_LT_VERSION_INFO)
