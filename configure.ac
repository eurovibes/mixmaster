dnl Every other copy of the package version number gets its value from here
AC_INIT(mixmaster, 3.2.0, mixmaster+dev@eurovibes.org)
AM_INIT_AUTOMAKE

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER(config.h)

AC_SUBST(VERSION)

ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_USE_SYSTEM_EXTENSIONS

dnl Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_YACC

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADER(ncurses.h)

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_CHECK_FUNCS([getdomeinname gethostname gettimeofday uname  ],,)

dnl pkg-config section
PKG_CHECK_MODULES(SSL, libcrypto,
  AC_DEFINE(USE_OPENSSL,, [use OpenSSL]),
  AC_MSG_ERROR([OpenSSL not found.])
)
PKG_CHECK_MODULES(ZLIB, zlib,
  AC_DEFINE(USE_ZLIB,, [use zlib]),
  AC_MSG_ERROR([zlib not found.])
)

AC_ARG_ENABLE(idea,
  AS_HELP_STRING([--enable-idea], [Build with IDEA crypto support]))
AS_IF([test "x$enable_idea" = "xyes"], [
  PKG_CHECK_MODULES(IDEA, libidea,
    AC_DEFINE(USE_IDEA,, [IDEA crypto support]),)
])

AC_ARG_ENABLE(libpcre,
  AS_HELP_STRING([--enable-libpcre], [use PERL regular expression library]))
AS_IF([test "x$enable_libpcre" != "xno"], [
  PKG_CHECK_MODULES(PCRE, libpcre,
    AC_DEFINE(USE_PCRE,, [use PERL regular expression library]),)
])

AC_ARG_ENABLE(ncurses,
  AS_HELP_STRING([--enable-ncurses], [use ncurses]))
AS_IF([test "x$enable_ncurses" != "xno"], [
  PKG_CHECK_MODULES(NCURSES, ncurses,
    AC_DEFINE(USE_NCURSES,, [Build with ncurses support]))
])

dnl flag section
AC_ARG_ENABLE(debug,
  AS_HELP_STRING([--enable-debug],
    [Build with additional debug functionality]))
AS_IF([test "x$enable_idea" = "xyes"], [
  AC_DEFINE(DEBUG,, [enable additional debug functionality])
],[
  AC_DEFINE(NDEBUG,, [disable additional debug functionality])
])

AC_ARG_ENABLE(sock,
  AS_HELP_STRING([--enable-sock], [Use socket to deliver mail]))
AS_IF([test "x$enable_sock" != "xno"], [
  AC_DEFINE(USE_SOCK,, [deliver mail by socket])
])

AC_ARG_ENABLE(aes,
  AS_HELP_STRING([--enable-aes], [Build with AES crypto support]))
AS_IF([test "x$enable_aes" != "xno"], [
  AC_DEFINE(USE_AES,, [AES crypto support])
])

AC_ARG_ENABLE(pgp,
  AS_HELP_STRING([--enable-pgp], [Build with OpenPGP compatiblity support]))
AS_IF([test "x$enable_pgp" != "xno"], [
  AC_DEFINE(USE_PGP,, [OpenPGP compatiblity support])
])

dnl default path section
AC_ARG_VAR(GLOBALMIXCONF, [path to global mixmaster configuration])
AS_IF([test "x$GLOBALMIXCONF" != "x"], [
  AC_DEFINE_UNQUOTED(GLOBALMIXCONF, ["$GLOBALMIXCONF"] ,
    [global mixmaster configuration])
])

AC_ARG_VAR(HOMEMIXDIR, [path to mixmaster user home])
AS_IF([test "x$HOMEMIXDIR" != "x"], [
  AC_DEFINE_UNQUOTED(HOMEMIXDIR, ["$HOMEMIXDIR"] ,[mixmaster user home])
])

dnl i18n makros.

AC_OUTPUT([Makefile
	Src/Makefile])
