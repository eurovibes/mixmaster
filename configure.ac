# Process this file with autoconf to produce a configure script.
# $Id: configure.ac,v 1.2 2003/10/14 14:58:51 dybbuk Exp $
AC_INIT(Src/version.h)
AC_CONFIG_AUX_DIR(etc)
AM_INIT_AUTOMAKE(mixmaster, [3.08+cvs], mixmaster-devel@lists.sourceforge.net)
AC_CONFIG_HEADER([config.h])

dnl Find system type
AC_CANONICAL_HOST

dnl Remove trailing slash in $prefix
case "${prefix}" in
  */)
    prefix=`echo ${prefix} | sed -e 's/^\(.*\)\/$/\1/'`
    ;;
esac

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_CPP

# These are all Windows libraries -- autoscan said we'd need them, but
# I have no Windows machine to test these on, so this undoubtedly is
# broken.  I will leave it up to a windows geek to figure this out.
dnl AC_CHECK_LIB([gdi32], [main])
dnl AC_CHECK_LIB([user32], [main])
dnl AC_CHECK_LIB([wsock32], [main])


# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h errno.h fcntl.h limits.h malloc.h memory.h     \
                  netdb.h netinet/in.h stddef.h stdlib.h string.h sys/file.h \
		  sys/socket.h sys/time.h termios.h unistd.h utime.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_STRFTIME
AC_FUNC_UTIME_NULL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([stat mktime fork atexit dup2 ftruncate getcwd gethostbyaddr \
		gethostbyname gethostname gettimeofday memmove memset mkdir \
		munmap socket strchr strdup strerror strrchr strstr strtol  \
        	uname utime setenv getdomainname])

# ncurses?
AH_TEMPLATE(
 [USE_NCURSES],
 [Define to 1 if you want ncurses support and have the ncurses library.]
)
AC_CHECK_LIB([ncurses], [getstr],
 [ LIBS="-lncurses ${LIBS}"
   AC_DEFINE(USE_NCURSES) ])
AC_CHECK_HEADERS(ncurses.h)

# Zlib check.  If the system version is older than 1.1.4, we want to
# use the included version.  1.1.4 fixed that nasty double-free bug.
dnl AM_ZLIB_CHECK
AH_TEMPLATE(
 [USE_ZLIB],
 [Define to 1 if you have ZLib.]
)
AC_CHECK_LIB([z], [gzopen],
 [ LIBS="-lz ${LIBS}"
   AC_DEFINE(USE_ZLIB) ],
 [ AC_MSG_ERROR([could not find a working zlib]) ]
)

# If PCRE isn't installed on the system, we luckily have included a
# version.
AH_TEMPLATE(
 [USE_PCRE],
 [Define to 1 if you have libpcre installed.]
)
AC_CHECK_LIB([pcre], [pcre_compile],
 [ LIBS="-lpcre ${LIBS}"
   AC_DEFINE(USE_PCRE) ],
 [ AC_MSG_ERROR([could not find a working libpcre]) ]
)

for dir in /usr/include /usr/local/include
do
    if test -d "$dir/pcre"
    then
       CPPFLAGS="-I$dir/pcre ${CPPFLAGS}"
       break
    fi
done
AC_CHECK_HEADER(pcre.h)

AM_OPENSSL_CHECK
AM_OPENSSL_RSA
AM_OPENSSL_IDEA
AM_OPENSSL_AES

# This is always defined, as near as I can tell.
AH_TEMPLATE([USE_PGP],
            [Define to support the OpenPGP message format.])
AC_DEFINE(USE_PGP)

# Did we set a passphrase, against our better judgement?
AM_PASSPHRASE

# Where should Mixmaster work its lovely magic?  Apparently this has a
# sane default now?
AM_MIXMASTER_DIR
AM_MIXMASTER_SPOOL
AM_MIXMASTER_HOME
AM_MIXMASTER_CONF

# Contains additional build information (such as pcre and zlib)
AC_SUBST(TOPDIRS)
AC_SUBST(XLIBS)
AC_SUBST(XDEFS)

AC_OUTPUT([Src/Makefile Makefile], [ echo timestamp > stamp-h ])
