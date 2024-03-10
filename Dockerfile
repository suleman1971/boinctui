FROM alpine AS base

RUN \
    apk add \
        automake autoconf g++ make \
        ncurses-dev expat-dev openssl-dev

COPY ./ /workdir/
WORKDIR /workdir

RUN \
    autoconf; \
    automake --add-missing; \
    ./configure --without-gnutls; \
    make; \
    strip boinctui

FROM alpine AS final

RUN \
    apk add --no-cache \
        expat ncurses openssl libstdc++ \
        libpanelw libformw libmenuw

COPY --from=base /workdir/boinctui /
ENTRYPOINT ["/boinctui"]
