if BUILD_EXAMPLES

BUILT_SOURCES = \
	static.c \
	static.h

clean-local:
	rm -f *.h *.c

%.c: %.xml
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --target $@ $<

%.h: %.xml
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --generate --target $@ $<

endif
