if BUILD_EXAMPLES

BUILT_SOURCES = \
	hello.c \
	hello.h \
	form.c \
	form.h

clean-local:
	rm -f *.h *.c

%.c: %.html ../../bin/balde-template-gen
	../../bin/balde-template-gen $< $@

%.h: %.html ../../bin/balde-template-gen
	../../bin/balde-template-gen $< $@

endif
