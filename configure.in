#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([boinctui], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([src])
#AC_CONFIG_HEADER([config.h:config.h.in])
#AC_CONFIG_HEADERS([config])

AC_PREFIX_DEFAULT(/usr)
#AC_PREFIX_PROGRAM(make)
AC_SYS_LARGEFILE

# Checks for programs.

AC_PROG_CXX
#AC_PROG_CC
#AC_PROG_CPP

# Checks for libraries.

AC_ARG_WITH(
    [ncursesw],
    [AS_HELP_STRING([--without-ncursesw],[fallback to plain ncurses (disable utf8 support)])],
    [
	AC_CHECK_LIB(ncurses, main,  , [AC_MSG_ERROR([Couldn't find ncurses library])] )
	AC_CHECK_LIB(form,    main,  , [AC_MSG_ERROR([Couldn't find form library])] )
	AC_CHECK_LIB(panel,   main,  , [AC_MSG_ERROR([Couldn't find panel library])] )
	AC_CHECK_LIB(menu,    main,  , [AC_MSG_ERROR([Couldn't find menu library])] )
    ],
    [
	#use unicode ncursesw (default case)
	AC_CHECK_LIB(ncursesw, main,  , [AC_MSG_ERROR([Couldn't find ncursesw library])] )
	#AC_CHECK_LIB(ncurses++w, main,  , [AC_MSG_ERROR([Couldn't find ncurses++w library])] )
	AC_CHECK_LIB(formw,    main,  , [AC_MSG_ERROR([Couldn't find formw library])] )
	AC_CHECK_LIB(panelw,   main,  , [AC_MSG_ERROR([Couldn't find panelw library])] )
	AC_CHECK_LIB(menuw,    main,  , [AC_MSG_ERROR([Couldn't find menuw library])] )
    ]
)

AC_CHECK_LIB(expat,    main,  , [AC_MSG_ERROR([Couldn't find expat library])] )
AC_CHECK_LIB(pthread,    main,  , [AC_MSG_ERROR([Couldn't find pthread library])] )

AC_ARG_WITH(
    [gnutls],
    [AS_HELP_STRING([--without-gnutls],[use openssl instead gnutls-openssl])],
    [
	HAVE_OPENSSL=1
	AC_SUBST([HAVE_OPENSSL])
	AC_CHECK_LIB(crypto ,  MD5_Init,  , [AC_MSG_ERROR([Couldn't find crypto library])] )
    ],
    [
	#use gnutls-openssl (default case)
	AC_CHECK_LIB(gnutls-openssl ,  MD5_Init,  , [AC_MSG_ERROR([Couldn't find gnutls-openssl library])] )
	#AC_CHECK_LIB(gnutls ,  MD5_Init,  , [AC_MSG_ERROR([Couldn't find gnutls library])] )
    ]
)


# Checks for header files.

AC_LANG_PUSH([C++])
AC_CHECK_HEADERS( \
[ \
pthread.h \
string.h stdlib.h stdio.h sys/stat.h unistd.h stdarg.h sys/ioctl.h arpa/inet.h \
locale.h malloc.h sys/socket.h netdb.h signal.h ctype.h \
algorithm string list vector queue stack sstream \
expat.h \
] ,  , [AC_MSG_ERROR([Couldn't find some headers])] )

AC_ARG_WITH(
    [gnutls],
    [AS_HELP_STRING([--without-gnutls],[use openssl instead gnutls-openssl])],
    [
	AC_CHECK_HEADERS( \
	[ openssl/md5.h ] ,  , [AC_MSG_ERROR([Couldn't find openssl/md5.h header])] )
    ],
    [
	#use gnutls-openssl (default case)
	AC_CHECK_HEADERS( \
	[ gnutls/openssl.h ] ,  , [AC_MSG_ERROR([Couldn't find gnutls/openssl.h header])] )
    ]
)

AC_ARG_WITH(
    [ncursesw],
    [AS_HELP_STRING([--without-ncursesw],[use ncurses instead ncursesw])],
    [
	#use old curses
	AC_CHECK_HEADERS( \
	[ curses.h form.h panel.h menu.h] ,  , [AC_MSG_ERROR([Couldn't find some curses header])] )
    ],
    [
	#use ncursesw (default case)
	AC_CHECK_HEADERS( \
	    [ ncursesw/curses.h ncursesw/form.h ncursesw/panel.h ncursesw/menu.h] ,
	    [
		AC_DEFINE(NCURSESW_HAVE_SUBDIR,1)
	    ],
	    #cursesw headers are in default include dir?
	    AC_CHECK_HEADERS( \ 
		[ curses.h form.h panel.h menu.h] ,
		[],
		[
		    AC_MSG_ERROR([Couldn't find some ncursesw header])
		] )
	)
    ]
)


AC_LANG_POP([C++])

# Checks for typedefs, structures, and compiler characteristics.

AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.

AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([mblen memset setlocale socket strdup strstr])

# Output files

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
