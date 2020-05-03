/*
 * Transitional place for setup stuff.
 * Will be droped after introducing autotools.
 */

#define VERSION "3.1-alpha3-4096"

/* Use the OpenSSL crypto library (required) */
#define USE_OPENSSL
/* Use IDEA algorithm? (See file idea.txt) */
/* #define USE_IDEA */
/* Use AES algorithm? */
#define USE_AES
/* Support the OpenPGP message format? */
#define USE_PGP

#ifdef WIN32
/* Use the PCRE regular expression library for destination blocking? */
#define USE_PCRE
/* Use zlib for compression? */
#define USE_ZLIB
/* Use ncurses? */
#define USE_NCURSES
/* Use the WIN GUI? */
#define USE_WINGUI
/* Use sockets to deliver mail */
#define USE_SOCK
#endif /* WIN32 */

#ifdef UNIX
#define HAVE_UNAME
#endif

#if defined(POSIX) || defined(USE_SOCK)
#define HAVE_GETHOSTNAME
#endif

#ifdef POSIX
/* not a POSIX function, but avaiable on virtually all Unix systems */
#define HAVE_GETTIMEOFDAY
#endif

#ifdef linux
#define HAVE_GETDOMAINNAME
#endif
