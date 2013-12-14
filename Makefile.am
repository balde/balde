SUBDIRS = \
	lib \
	include \
	examples \
	tests

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

valgrind:
	$(MAKE) check TESTS_ENVIRONMENT=" \
		G_SLICE=always-malloc \
		G_DEBUG=gc-friendly \
		libtool \
			--mode=execute \
			valgrind \
				--tool=memcheck \
				--leak-check=full \
				--leak-resolution=high \
				--num-callers=20"
