AC_INIT([boinctui],
        [2.7.2],
        [https://github.com/suleman1971/boinctui/issues])

AM_INIT_AUTOMAKE([foreign -Wno-portability])

AC_PROG_CXX

PKG_CHECK_MODULES(BOINCTUI, [
  ncursesw
  panelw
  formw
  menuw
  expat
])

AC_ARG_WITH(
  [gnutls],
  AS_HELP_STRING([--without-gnutls],
  [use OpenSSL instead of GnuTLS-OpenSSL]),
  [
    CXXFLAGS="$CXXFLAGS -DHAVE_OPENSSL"
    PKG_CHECK_MODULES(CRYPTO, [openssl])
  ],
  [
    PKG_CHECK_MODULES(CRYPTO, [gnutls])
  ]
)

AC_ARG_WITH(
	[boinc-dir],
	[AS_HELP_STRING([--with-boinc-dir=DIR], [Specify the BOINC data directory])],
	[CXXFLAGS="$CXXFLAGS -DBOINC_DIR=$withval"],
	[CXXFLAGS="$CXXFLAGS -DBOINC_DIR=/var/lib/boinc-client"]
)

AC_CONFIG_FILES([
  Makefile
  src/Makefile
])

AC_OUTPUT
