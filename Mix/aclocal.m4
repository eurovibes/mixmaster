dnl Various specific config bits for Mixmaster.
dnl
dnl $Id: aclocal.m4,v 1.2 2003/08/07 15:15:35 dybbuk Exp $

dnl Zlib versions before 1.1.4 had a nasty bug.
AC_DEFUN(AM_ZLIB_CHECK, [
  zlib_ok=0
  AC_MSG_CHECKING(for zlib 1.1.4)
  AC_RUN_IFELSE([#include <zlib.h>
#include <string.h>

int main(void)
{
#if !defined(ZLIB_VERSION)
exit(1);
#else
if (strcmp(ZLIB_VERSION, "1.1.4") >= 0)
  exit(0);
else
  exit(1);
#endif
}
  ],
  [ AC_MSG_RESULT(ok)
    zlib_ok=1
    LIBS="-lz ${LIBS}"
    CPPFLAGS="-DUSE_ZLIB ${CPPFLAGS}"
  ],
  [
    AC_MSG_RESULT([no ... use the included Zlib.])
    AC_MSG_CHECKING(for included zlib)
    if test -d "Src/zlib-1.1.4"
    then
       AC_MSG_RESULT(ok)
    else
       AC_MSG_ERROR([Library not found.])
    fi
    CPPFLAGS="-DUSE_ZLIB -Izlib-1.1.4 ${CPPFLAGS}"
    TOPDIRS="Src/zlib-1.1.4 ${TOPDIRS}"
    XLIBS="zlib-1.1.4/libz.a ${XLIBS}"
  ], [])
])

AC_DEFUN(AM_PASSPHRASE, [
AC_ARG_WITH(passphrase,
[  --with-passphrase=PASS  Set Mixmaster key passphrase (NOT RECOMMENDED!)],
  [ CPPFLAGS="-DPASSPHRASE='\"${with_passphrase}\"' ${CPPFLAGS}"
    AC_MSG_WARN([You have set a passphrase in a bad place!  Please read the docs!]) ],
  [])
])

dnl This will detect our OpenSSL version.  Bits are stolen from the
dnl old crufty Install script.
AC_DEFUN(AM_OPENSSL_CHECK, [
# Check our OpenSSL installation.
AC_ARG_WITH(ssl-dir,
[  --with-ssl-dir=PATH     Specify path to OpenSSL installation ],
 [
if test "x$withval" != "xno" ; then
    tryssldir=$withval
fi
 ])
# Backup our variables.
saved_LIBS="$LIBS"
saved_LDFLAGS="$LDFLAGS"
saved_CPPFLAGS="$CPPFLAGS"
# Find our real SSL directory!
if test "x$prefix" != "xNONE" ; then
        tryssldir="$tryssldir $prefix"
fi
AC_CACHE_CHECK([for OpenSSL directory], ac_cv_openssldir, [
for ssldir in $tryssldir "" /usr /usr/local/openssl /usr/lib/openssl /usr/local/ssl /usr/lib/ssl /usr/local /usr/pkg /opt /opt/openssl ; do
    CPPFLAGS="$saved_CPPFLAGS"
    LDFLAGS="$saved_LDFLAGS"
    LIBS="$saved_LIBS -lcrypto"

    # Skip non-existant directories.
    test -d "$ssldir" || continue

    if test "x$ssldir" != "x/usr"; then
        if test -d "$ssldir/lib"; then
            LDFLAGS="-L$ssldir/lib $saved_LDFLAGS"
        else
            LDFLAGS="-L$ssldir $saved_LDFLAGS"
        fi
        if test -d "$ssldir/include"; then
            CPPFLAGS="-I$ssldir/include $saved_CPPFLAGS"
        else
            CPPFLAGS="-I$ssldir $saved_CPPFLAGS"
        fi
    fi

    # So ... can we link with SSL here or not?  Note that the IDEA and
    # AES checks come later.
    AC_TRY_RUN([
#include <string.h>
#include <openssl/rand.h>
int main(void) 
{
        char a[2048];
        memset(a, 0, sizeof(a));
        RAND_add(a, sizeof(a), sizeof(a));
        return(RAND_status() <= 0);
}
    ], [ have_openssl=1; break ], [ ])

    test -z "$have_openssl" && break
done
ac_cv_openssldir="$ssldir"
])

# The above test doesn't get everything working for us.  Now we work
# directly with the cache value.
if (test ! -z "$ac_cv_openssldir" && test "x$ac_cv_openssldir" != "x(system)") ; then
    AC_DEFINE(HAVE_OPENSSL)
    dnl Need to recover ssldir - test above runs in subshell
    ssldir=$ac_cv_openssldir
    if test ! -z "$ssldir" -a "x$ssldir" != "x/usr"; then
         # Try to use $ssldir/lib if it exists, otherwise 
         # $ssldir
         if test -d "$ssldir/lib" ; then
             LDFLAGS="-L$ssldir/lib $saved_LDFLAGS"
             if test ! -z "$need_dash_r" ; then
                 LDFLAGS="-R$ssldir/lib $LDFLAGS"
             fi
         else
             LDFLAGS="-L$ssldir $saved_LDFLAGS"
             if test ! -z "$need_dash_r" ; then
                 LDFLAGS="-R$ssldir $LDFLAGS"
             fi
         fi
         # Try to use $ssldir/include if it exists, otherwise 
         # $ssldir
         if test -d "$ssldir/include" ; then
             CPPFLAGS="-I$ssldir/include $saved_CPPFLAGS"
         else
             CPPFLAGS="-I$ssldir $saved_CPPFLAGS"
         fi
    fi
fi
LIBS="$saved_LIBS -lcrypto"
])

dnl Run AM_OPENSSL_CHECK before these or they will fail!
AC_DEFUN(AM_OPENSSL_RSA, [
AC_MSG_CHECKING(for RSA support)
AC_TRY_RUN([
#include <string.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/sha.h>
int main(void) 
{
        int num; RSA *key; static unsigned char p_in[] = "blahblah";
        unsigned char c[256], p[256];
        memset(c, 0, sizeof(c)); RAND_add(c, sizeof(c), sizeof(c));
        if ((key=RSA_generate_key(512, 3, NULL, NULL))==NULL) return(1);
        num = RSA_public_encrypt(sizeof(p_in) - 1, p_in, c, key, RSA_PKCS1_PADDING);
        return(-1 == RSA_private_decrypt(num, c, p, key, RSA_PKCS1_PADDING));
}
 ], [ AC_MSG_RESULT(yes) ],
 [ AC_MSG_ERROR(RSA support not found!) ])
])

AC_DEFUN(AM_OPENSSL_IDEA, [
AC_ARG_ENABLE(idea,
  [  --disable-idea          Disable IDEA support (autodetected) ])
# Are we going without?
if test -z "$enable_idea" -o "$enable_idea" = "yes"; then
    AC_MSG_CHECKING(for IDEA support)
    AC_TRY_RUN([
#include <openssl/idea.h>
#include <openssl/evp.h>
int main() {
  if(EVP_get_cipherbyname("idea-cbc") != NULL)
    exit(0);
  else  
    exit(1);
}
 ], [
    AC_MSG_RESULT(yes)
    CPPFLAGS="-DUSE_IDEA $CPPFLAGS"
 ],
 [ AC_MSG_ERROR(IDEA support not found!
    Please explicitly disable it with --disable-idea and read the
    documentation to find out what will be broken or missing if IDEA
    support is disabled.) ])
fi

])

AC_DEFUN(AM_OPENSSL_AES, [
AC_ARG_ENABLE(openssl_aes,
[  --disable-aes           Enable AES support (autodetected) ])

# Where oh where has my little AES gone?
if test -z "$enable_aes" -o "$enable_aes" = "yes"; then
    AC_MSG_CHECKING(for AES support)
    AC_TRY_RUN([
#include <openssl/aes.h>
#include <openssl/evp.h>
int main() {
  if(EVP_get_cipherbyname("aes-128-cbc") != NULL)
    exit(0);
  else
    exit(1);
}
 ], [
    AC_MSG_RESULT(yes)
    CPPFLAGS="-DUSE_AES $CPPFLAGS"
 ],
 [ AC_MSG_RESULT(no) ])
fi

])

dnl Peter Palfrader suggested that no default directory be set here
dnl unless explicitly commanded!  I like that idea.
AC_DEFUN(AM_MIXMASTER_DIR, [
MIXDIR=""
AC_MSG_CHECKING(for Mixmaster directory)
AC_ARG_WITH(mixdir,
  [  --with-mixdir           Set default Mix directory ],
  [ if test -d "$mixdir"
    then
       AC_MSG_RESULT($mixdir)
       MIXDIR="-DSPOOL='\"$mixdir\"'"
    else
       AC_MSG_RESULT([not found, using default])
    fi
  ],
  [ AC_MSG_RESULT(using default) ])
AC_SUBST(MIXDIR)
])
      
dnl HOMEMIXDIR is something else.
AC_DEFUN(AM_HOMEMIXDIR, [
HOMEMIXDIR=""
AC_MSG_CHECKING(for default relative Mix directory)
AC_ARG_WITH(homemixdir,
  [  --with-homemixdir=DIR   Default Mix directory relative to \$HOME ],
  [ HOMEMIXDIR="$with_homemixdir"
    CPPFLAGS="-DHOMEMIXDIR='\"$HOMEMIXDIR\"' ${CPPFLAGS}"
    AC_MSG_RESULT(["\$HOME/${HOMEMIXDIR}"]) ],
  [ AC_MSG_RESULT(using default) ])
])

dnl Is the global mixmaster config working yet?
AC_DEFUN(AM_MIX_CONF, [
MIXCONF=""
AC_MSG_CHECKING(for global configuration file)
AC_ARG_WITH(global_conf,
  [  --with-global-conf=FILE Global Mixmaster configuration (no default) ],
  [ CPPFLAGS="-DGLOBALMIXCONF=\"'$global_conf'\" ${CPPFLAGS}"
    AC_MSG_RESULT([$global_conf]) ],
  [ AC_MSG_RESULT(none) ])
])

