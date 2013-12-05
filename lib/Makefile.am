lib_LTLIBRARIES = libbalde.la

libbalde_la_SOURCES = \
	balde.c \
	balde.h \
	routing.c \
	routing.h

libbalde_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)

libbalde_la_LDFLAGS = \
	$(AM_LDADD) \
	-version-info $(LT_VERSION_INFO)
