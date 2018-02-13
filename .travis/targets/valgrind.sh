build() {
    pushd build > /dev/null
    ../configure \
        CFLAGS="-Wall -g -O0" \
        --disable-silent-rules \
        --enable-examples \
        --enable-http \
        --enable-leg \
        --enable-valgrind \
        --disable-doc
    popd > /dev/null

    make -C build valgrind
}
