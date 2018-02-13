build() {
    pushd build > /dev/null
    ../configure \
        CFLAGS="-O0 -g -Wall" \
        --disable-silent-rules \
        --enable-examples \
        --enable-http \
        --enable-leg \
        --disable-valgrind \
        --disable-doc
    popd > /dev/null

    make -C build distcheck
}

deploy() {
    FILES=( build/*.{*.tar.{gz,bz2,xz},zip} )
}
