SUBDIRS = \
	artwork \
	libbalde \
	libbalde_template \
	include \
	bin \
	examples \
	doc \
	tests

ACLOCAL_AMFLAGS = -I m4
AM_DISTCHECK_CONFIGURE_FLAGS = \
	--enable-examples \
	--without-leg

EXTRA_DIST = \
	$(top_srcdir)/autogen.sh \
	$(top_srcdir)/.version \
	$(top_srcdir)/build-aux/git-version-gen \
	$(top_srcdir)/README.md

BUILT_SOURCES = \
	$(top_srcdir)/.version

$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = balde.pc

if BUILD_DOCS
doxygen: Doxyfile
	$(MAKE) -C doc clean
	$(DOXYGEN) Doxyfile

if PUBLISH_DOCS
publish_docs: doxygen
	$(GITHUB_PAGES_PUBLISH) --verbose \
		--cname docs.balde.io \
		. doc/build/html
endif
endif

if USE_VALGRIND
valgrind: all
	$(MAKE) -C tests valgrind
endif
