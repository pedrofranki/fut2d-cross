#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([fcportugal_setplay],[0.1],[lhrmota@gmail.com])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

# --------------------------------------------------------------
AM_INIT_AUTOMAKE([gnu check-news])

AC_LANG([C++])

# --------------------------------------------------------------
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

# ----------------------------------------------------------
# check boost
AX_BOOST_BASE([1.32.0])
CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"

# check librcsc path
AC_SUBST(LIBRCSCLIB)
LIBRCSCLIB="/usr/local/lib"
librcsc_prefix=""
AC_ARG_WITH(librcsc,
            [  --with-librcsc=PREFIX prefix where librcsc is installed (optional)],
            librcsc_prefix="$withval",
            librcsc_prefix="")
if test x$librcsc_prefix != x; then
  CPPFLAGS="$CPPFLAGS -I$librcsc_prefix/include"
  LDFLAGS="$LDFLAGS -L$librcsc_prefix/lib"
  LIBRCSCLIB="$librcsc_prefix/lib"
else
  for librcsc_path_tmp in $HOME $HOME/local $HOME/rcss /opt/robocup /opt /opt/local ; do
    if test -d "$librcsc_path_tmp/include/rcsc" && test -r "$librcsc_path_tmp/include/rcsc/types.h" ; then
      CPPFLAGS="$CPPFLAGS -I$librcsc_path_tmp/include"
      LDFLAGS="$LDFLAGS -L$librcsc_path_tmp/lib"
      LIBRCSCLIB="$librcsc_path_tmp/lib"
      break;
    fi
  done
fi

# ----------------------------------------------------------------------------
# Check for Setplay Library
# assuming it is located in the upper directory, under SetplayFramework/
# ----------------------------------------------------------------------------

AC_ARG_WITH(
    setplaydir,
    AS_HELP_STRING(
	[--with-setplaydir=DIR],
	[FCPortugal Setplay directory, with libsetplay.a in the lib folder]
    ),
    [
    SETPLAY_LOCATION="$withval"
	fcportugal_setplay_player_CXXFLAGS="-W -Wall -I"$withval""
	fcportugal_setplay_player_LDADD="-L"$withval"/lib/ -lsetplay"	
    ],
    [AC_MSG_ERROR([Setplay library dir must be entered as argument with --with-setplaydir=DIR ])])
 
# Check if path is absolute
if test `echo ${SETPLAY_LOCATION} | grep '^/'`; then
	echo "Setplay dir OK"
else   
    AC_MSG_ERROR([Setplay location must be an absolute path])
fi
AC_SUBST(fcportugal_setplay_player_CXXFLAGS)
AC_SUBST(fcportugal_setplay_player_LDADD)

AC_CHECK_FILE($SETPLAY_LOCATION/setplay/setplay.h, 
    [found_setplay_header=yes; break;])
    
AS_IF([test "x$found_setplay_header" != "xyes"],
    [AC_MSG_ERROR([Could not find setplay header: $SETPLAY_LOCATION/setplay/setplay.h])])
    
# --------------------------------------------------------------
# Checks for libraries.
AC_CHECK_LIB([m], [cos],
             [LIBS="-lm $LIBS"],
             [AC_MSG_ERROR([*** -lm not found! ***])])
AC_CHECK_LIB([z], [deflate])

AC_CHECK_LIB([rcsc_geom], [main],
             [LIBS="-lrcsc_geom $LIBS"],
             [AC_MSG_ERROR([*** -lrcsc_geom not found! ***])])
AC_CHECK_LIB([rcsc_rcg], [main],
             [LIBS="-lrcsc_rcg $LIBS"],
             [AC_MSG_ERROR([*** -lrcsc_rcg not found! ***])])
# defined HAVE_LIBRCSC_GZ and set LIBS="-lrcsc_gz $LIBS"
AC_CHECK_LIB([rcsc_gz], [main])
AC_CHECK_LIB([rcsc_param], [main],
             [LIBS="-lrcsc_param $LIBS"],
             [AC_MSG_ERROR([*** -lrcsc_param not found! ***])])
AC_CHECK_LIB([rcsc_time], [main],
             [LIBS="-lrcsc_time $LIBS"],
             [AC_MSG_ERROR([*** -lrcsc_time not found! ***])])
AC_CHECK_LIB([rcsc_net], [main],
             [LIBS="-lrcsc_net $LIBS"],
             [AC_MSG_ERROR([*** -lrcsc_time not found! ***])])
AC_CHECK_LIB([rcsc_ann], [main],
             [LIBS="-lrcsc_ann $LIBS"],
             [AC_MSG_ERROR([*** -lrcsc_ann not found! ***])])
AC_CHECK_LIB([rcsc_agent], [main],
             [LIBS="-lrcsc_agent $LIBS"],
             [AC_MSG_ERROR([*** -lrcsc_agent not found! ***])])
             


# --------------------------------------------------------------
# Checks for header files.
AC_CHECK_HEADERS([rcsc/types.h])

# --------------------------------------------------------------
# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

# --------------------------------------------------------------
# Add original options
AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug],
                             [turn on debugging (default=no)]))
if test "x$enable_debug" = "xyes"; then
#  AC_DEFINE([DEBUG], [1], [debugging flag])
  CFLAGS="-DDEBUG $CFLAGS"
  CXXFLAGS="-DDEBUG $CXXFLAGS"
  echo "enabled debug"
fi

# --------------------------------------------------------------
# Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS([floor pow sqrt strerror])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/start.sh
                 src/keepaway.sh
                 src/train.sh],
                 [test -f src/start.sh && chmod +x src/start.sh]
                 [test -f src/keepaway.sh && chmod +x src/keepaway.sh]
                 [test -f src/train.sh && chmod +x src/train.sh])
AC_OUTPUT
