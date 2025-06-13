FROM alpine AS base

RUN \
    apk add \
        cmake g++ make \
        ncurses-dev expat-dev openssl-dev

COPY ./ /workdir/
WORKDIR /workdir

RUN \
    mkdir -p build; \
    cd build; \
    cmake -DWITHOUT_GNUTLS=true ..; \
    make; \
    strip boinctui

FROM alpine AS final

RUN \
    apk add --no-cache \
        expat ncurses openssl libstdc++ \
        libpanelw libformw libmenuw

COPY --from=base /workdir/build/boinctui /
ENTRYPOINT ["/boinctui"]
