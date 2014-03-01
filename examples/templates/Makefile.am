if BUILD_EXAMPLES

BUILT_SOURCES = \
	hello.c \
	hello.h \
	form.c \
	form.h

clean-local:
	rm -f *.h *.c

%.c: %.html $(top_builddir)/src/bin/balde-template-gen
	$(AM_V_GEN)$(top_builddir)/src/bin/balde-template-gen $< $@

%.h: %.html $(top_builddir)/src/bin/balde-template-gen
	$(AM_V_GEN)$(top_builddir)/src/bin/balde-template-gen $< $@

endif
