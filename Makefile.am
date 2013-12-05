SUBDIRS = src
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	$(top_srcdir)/autogen.sh \
	$(top_srcdir)/.version

BUILT_SOURCES = \
	$(top_srcdir)/.version

$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version
