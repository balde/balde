SUBDIRS = \
	lib \
	include \
	bin \
	examples \
	doc \
	tests

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	$(top_srcdir)/autogen.sh \
	$(top_srcdir)/.version \
	$(top_srcdir)/README.md

BUILT_SOURCES = \
	$(top_srcdir)/.version

$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

if BUILD_DOCS
doxygen: Doxyfile
	$(MAKE) -C doc clean
	$(DOXYGEN) Doxyfile

if PUBLISH_DOCS
publish_docs: doxygen
	$(GITHUB_PAGES_PUBLISH) --verbose \
		--cname balde.io \
		. doc/build/html
endif
endif

if USE_VALGRIND
valgrind: all
	$(MAKE) -C tests valgrind
endif
