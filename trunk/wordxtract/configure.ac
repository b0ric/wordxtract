#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([wordxtract], [0.1], [b0ric.alex@gmail.com])
AC_INIT(wordxtract, 0.1, b0ric.alex@gmail.com)
AM_INIT_AUTOMAKE([-Wall -Werror dist-bzip2])
AC_CONFIG_SRCDIR([src/dict.h])
AC_CONFIG_HEADER([config.h])

# platform configuration
case "${host}" in
    i[[3456789]]86-mingw32*)
    WIN32="yes"
    #checking platform-dependent tools
    AC_CHECK_TOOL(WINDRES, windres)
    ;;
    *cygwin*) WIN32="yes";;
    *) WIN32="no" ;;
esac
AM_CONDITIONAL([WIN32], test "$WIN32" = "yes")

# i18n support
AC_PROG_INTLTOOL([0.23])
ALL_LINGUAS="ru"
AC_SUBST(ALL_LINGUAS)
GETTEXT_PACKAGE=wordxtract
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [The domain to use with gettext])
AM_GLIB_GNU_GETTEXT

wordxtractlocaledir='${datadir}/locale'
AC_SUBST(wordxtractlocaledir)

# Checks for gtk+-2.0.
AC_PROG_CC
AM_PATH_GTK_2_0([2.12.0])
PKG_CHECK_MODULES(GTK, gtk+-2.0,, exit)
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_MODE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([mkdir])

AC_CONFIG_FILES([
Makefile
data/Makefile
pixmaps/Makefile
po/Makefile.in
src/Makefile
src/pixmaps/Makefile
src/win32/Makefile
])
AC_OUTPUT
