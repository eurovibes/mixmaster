/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Configuration
   $Id: config.h,v 1.16 2002/08/26 19:38:50 weaselp Exp $ */


#ifndef _CONFIG_H
#define _CONFIG_H
#include "version.h"

/* Disclaimer to be inserted in all anonymous messages: */
#define DISCLAIMER \
 "Comments: This message did not originate from the Sender address above.\n" \
 "\tIt was remailed automatically by anonymizing remailer software.\n" \
 "\tPlease report problems or inappropriate use to the\n" \
 "\tremailer administrator at <%s>.\n"	/* (%s is the complaints address) */

/* Additional disclaimer to be inserted in the body of messages with
 * user-supplied From lines, e.g.
 * "NOTE: The above From: line has not been authenticated!\n\n" */
#define FROMDISCLAIMER ""

/* Additional disclaimer to be inserted at the bottom of the body of all
 * messages */
#define MSGFOOTER ""

/* Comment to be inserted when a binary attachment is filtered out: */
#define BINDISCLAIMER \
 "[...]"

/* Character set for MIME-encoded mail header lines */
#define MIMECHARSET "iso-8859-1"
#if 1
#define DEFLTENTITY ""
#else
#define DEFLTENTITY "text/plain; charset=" MIMECHARSET
#endif

/** Libraries and library functions **********************************/

/* Use the OpenSSL crypto library (required) */
#define USE_OPENSSL
/* Use the RSA cryptosystem? */
#define USE_RSA
/* Use IDEA algorithm? (See file idea.txt) */
/* #define USE_IDEA */
/* Use AES algorithm? - should be handled by Install script setting compiler option -DUSE_AES */
/* #define USE_AES */
/* Support the OpenPGP message format? */
#define USE_PGP

 /* the following are defined in the Makefile */
#if 0
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
#endif

/** System dependencies **********************************************/
/* Macros: UNIX  for Unix-style systems
           POSIX for systems with POSIX header files (including DJGPP)
           MSDOS for 32 bit DOS
           WIN32 for Windows 95/NT */

#if defined(_WIN32) && !defined(WIN32)
#define WIN32
#endif

#if defined(__RSXNT__) && !defined(WIN32)
#define WIN32
#endif

#if !defined(UNIX) && !defined(WIN32) && !defined(MSDOS)
#define UNIX
#endif

#if defined(UNIX) && !defined(POSIX)
#define POSIX
#endif

#ifdef UNIX
#define HAVE_UNAME
#define HAVE_GECOS
#define DEV_URANDOM "/dev/urandom"
#ifdef __OpenBSD__
#define DEV_RANDOM "/dev/srandom"
#else
#define DEV_RANDOM "/dev/random"
#endif
#endif

#if defined(POSIX) || defined(USE_SOCK)
#define HAVE_GETHOSTNAME
#endif

#ifdef POSIX
#define HAVE_TERMIOS
/* not a POSIX function, but avaiable on virtually all Unix systems */
#define HAVE_GETTIMEOFDAY
#endif

#ifdef linux
#define HAVE_GETDOMAINNAME
#endif

#ifdef MSDOS
#define SHORTNAMES
#ifndef WIN32
#define HAVE_GETKEY
#undef USE_SOCK
#endif
#endif

#if defined(USE_WINGUI) && !defined(WIN32)
#error "The GUI requires Win32!"
#endif

#if defined(WIN32) && !defined(_USRDLL)
#define DLLIMPORT __declspec(dllimport)
#else
#define DLLIMPORT
#endif

/** Constants *********************************************************/

/* Give up if a file is larger than BUFFER_MAX bytes: */
/* #define BUFFER_MAX 64*1024*1024 */

#define PATHMAX 512
#define LINELEN 128
#define BUFSIZE 4096

/** if it is a systemwide installation defined GLOBALMIXCONF **********/
/* #define GLOBALMIXCONF "/etc/mix.cfg" */

/* The path to append to a user's homedirectory for his local Mix dir */
#ifndef HOMEMIXDIR
#define HOMEMIXDIR "Mix"
#endif

/** file names ********************************************************/

#define DEFAULT_MIXCONF "mix.cfg"      /* mixmaster configuration file */
#define DEFAULT_DISCLAIMFILE "disclaim.txt"
#define DEFAULT_FROMDSCLFILE "fromdscl.txt"
#define DEFAULT_MSGFOOTERFILE "footer.txt"
#define DEFAULT_POP3CONF "pop3.cfg"
#define DEFAULT_HELPFILE "help.txt"
#define DEFAULT_ABUSEFILE "abuse.txt"
#define DEFAULT_REPLYFILE "reply.txt"
#define DEFAULT_USAGEFILE "usage.txt"
#define DEFAULT_USAGELOG "usage.log"
#define DEFAULT_BLOCKFILE "blocked.txt"
#define DEFAULT_ADMKEYFILE "adminkey.txt"
#define DEFAULT_KEYFILE "key.txt"
#define DEFAULT_PGPKEY "pgpkey.txt"
#define DEFAULT_DSAPARAMS "dsaparam.mix"
#define DEFAULT_DHPARAMS "dhparam.mix"
#define DEFAULT_MIXRAND "mixrand.bin"
#define DEFAULT_SECRING "secring.mix"
#define DEFAULT_PUBRING "pubring.mix"
#define DEFAULT_IDLOG "id.log"
#define DEFAULT_STATS "stats.log"
/* To enable multiple dest.blk files, edit the following line. */
/* Filenames must be seperated by one space.                   */
#define DEFAULT_DESTBLOCK "dest.blk rab.blk"
#define DEFAULT_DESTALLOW "dest.alw"
#define DEFAULT_SOURCEBLOCK "source.blk"
#define DEFAULT_HDRFILTER "header.blk"
#define DEFAULT_REGULAR "time.log"
#define DEFAULT_POOL "pool"            /* remailer pool subdirectory */
#define DEFAULT_TYPE1LIST "rlist.txt"
#define DEFAULT_TYPE2REL "mlist.txt"
#ifdef SHORTNAMES
#define DEFAULT_TYPE2LIST "type2.lis"
#else
#define DEFAULT_TYPE2LIST "type2.list"
#endif
  
#define DEFAULT_PGPREMPUBRING "pubring.pgp"
#define DEFAULT_PGPREMPUBASC "pubring.asc"
#define DEFAULT_PGPREMSECRING "secring.pgp"
#define DEFAULT_NYMSECRING "nymsec.pgp"
#define DEFAULT_NYMDB "secrets.mix"

extern char MIXCONF[];
extern char DISCLAIMFILE[];
extern char FROMDSCLFILE[];
extern char MSGFOOTERFILE[];
extern char POP3CONF[];
extern char HELPFILE[];
extern char ABUSEFILE[];
extern char REPLYFILE[];
extern char USAGEFILE[];
extern char USAGELOG[];
extern char BLOCKFILE[];
extern char ADMKEYFILE[];
extern char KEYFILE[];
extern char PGPKEY[];
extern char DSAPARAMS[];
extern char DHPARAMS[];
extern char MIXRAND[];
extern char SECRING[];
extern char PUBRING[];
extern char IDLOG[];
extern char STATS[];
extern char DESTBLOCK[];
extern char DESTALLOW[];
extern char SOURCEBLOCK[];
extern char HDRFILTER[];
extern char REGULAR[];
extern char POOL[];
extern char TYPE1LIST[];
extern char TYPE2REL[];
extern char TYPE2LIST[];

extern char PGPREMPUBRING[];
extern char PGPREMPUBASC[];
extern char PGPREMSECRING[];
DLLIMPORT extern char NYMSECRING[];
extern char NYMDB[];

/* string constants */
#define remailer_type "Remailer-Type: Mixmaster "
#define begin_remailer "-----BEGIN REMAILER MESSAGE-----"
#define end_remailer "-----END REMAILER MESSAGE-----"
#define begin_key "-----Begin Mix Key-----"
#define end_key "-----End Mix Key-----"
#define begin_pgp "-----BEGIN PGP "
#define end_pgp "-----END PGP "
#define begin_pgpmsg "-----BEGIN PGP MESSAGE-----"
#define end_pgpmsg "-----END PGP MESSAGE-----"
#define begin_pgpkey "-----BEGIN PGP PUBLIC KEY BLOCK-----"
#define end_pgpkey "-----END PGP PUBLIC KEY BLOCK-----"
#define begin_pgpseckey "-----BEGIN PGP PRIVATE KEY BLOCK-----"
#define end_pgpseckey "-----END PGP PRIVATE KEY BLOCK-----"
#define begin_pgpsigned "-----BEGIN PGP SIGNED MESSAGE-----"
#define begin_pgpsig "-----BEGIN PGP SIGNATURE-----"
#define end_pgpsig "-----END PGP SIGNATURE-----"
#define info_beginpgp "=====BEGIN PGP MESSAGE====="
#define info_endpgp "=====END PGP MESSAGE====="
#define info_pgpsig "=====Sig: "


/***********************************************************************
 * The following variables are read from mix.cfg, with default values
 * defined in mix.c */

int REMAIL;
int MIX;
int PGP;
int UNENCRYPTED;
int REMIX;
int REPGP;
extern char MIXDIR[];
extern char POOLDIR[];
extern char SENDMAIL[];
extern char SENDANONMAIL[];
extern char SMTPRELAY[];
extern char NEWS[];
extern char MAILtoNEWS[];
extern char ORGANIZATION[];
extern char MID[];
extern char TYPE1[];
extern char ERRLOG[];
extern char NAME[];
extern char ADDRESS[];
extern char REMAILERADDR[];
extern char ANONADDR[];
extern char REMAILERNAME[];
extern char ANONNAME[];
extern char COMPLAINTS[];
extern int AUTOREPLY;
extern char HELONAME[];
extern char ENVFROM[];
extern char SHORTNAME[];
extern int POOLSIZE;
extern int RATE;
extern int MIDDLEMAN;
extern int AUTOBLOCK;
extern char FORWARDTO[];
extern int SIZELIMIT;
extern int INFLATEMAX;
extern int MAXRANDHOPS;
extern int BINFILTER;
extern int LISTSUPPORTED;
extern long PACKETEXP;
extern long IDEXP;
DLLIMPORT extern int VERBOSE;
extern long SENDPOOLTIME;
extern long MAILINTIME;
extern int NUMCOPIES;
extern char CHAIN[];
extern int DISTANCE;
extern int MINREL;
extern int RELFINAL;
extern long MAXLAT;
DLLIMPORT extern char PGPPUBRING[];
DLLIMPORT extern char PGPSECRING[];
extern char PASSPHRASE[];
extern long POP3TIME;
extern int POP3DEL;
extern int POP3SIZELIMIT;
extern char MAILBOX[];
extern char MAILIN[];
extern char MAILABUSE[];
extern char MAILBLOCK[];
extern char MAILUSAGE[];
extern char MAILANON[];
extern char MAILERROR[];
extern char MAILBOUNCE[];

extern char ENTEREDPASSPHRASE[LINELEN];

#endif
