if BUILD_EXAMPLES

BUILT_SOURCES = \
	hello.c \
	hello.h \
	form.c \
	form.h

EXTRA_DIST = \
	hello.html \
	form.html

clean-local:
	rm -f *.h *.c

%.c: %.html $(top_builddir)/bin/balde-template-gen
	$(AM_V_GEN)$(top_builddir)/bin/balde-template-gen $< $@

%.h: %.html $(top_builddir)/bin/balde-template-gen
	$(AM_V_GEN)$(top_builddir)/bin/balde-template-gen $< $@

endif
