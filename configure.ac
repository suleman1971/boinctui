AC_INIT([boinctui],
        [2.5.0],
        [https://github.com/suleman1971/boinctui/issues])

AM_INIT_AUTOMAKE([-Wno-portability])

AC_PROG_CXX

PKG_CHECK_MODULES(BOINCTUI, [
  ncursesw
  panelw
  formw
  menuw
  expat
])

AC_ARG_WITH([gnutls], AS_HELP_STRING([--without-gnutls], [use OpenSSL instead of GnuTLS-OpenSSL]), [use_openssl=yes])

AS_IF([test "x$use_openssl" = "xyes"],
      [
        PKG_CHECK_MODULES(CRYPTO, [openssl])
      ],
      [
        PKG_CHECK_MODULES(CRYPTO, [gnutls])
      ]
)
AM_CONDITIONAL([USE_OPENSSL], [test "x$use_openssl" = "xyes"])


AC_CONFIG_FILES([
  Makefile
  src/Makefile
])

AC_OUTPUT
