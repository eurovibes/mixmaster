/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Configuration
   $Id: config.h,v 1.3 2001/12/12 19:06:41 rabbi Exp $ */


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

/* Comment to be inserted when a binary attachment is filtered out: */
#define BINDISCLAIMER \
 "[...]"

/* Passphrase used to protect our secret keys */
#ifndef PASSPHRASE
#define PASSPHRASE ""
#endif

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
#define USE_IDEA
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

/** Constants *********************************************************/

/* Give up if a file is larger than BUFFER_MAX bytes: */
/* #define BUFFER_MAX 64*1024*1024 */

#define PATHMAX 512
#define LINELEN 128
#define BUFSIZE 4096

/** file names ********************************************************/

#define MIXCONF "mix.cfg"	/* mixmaster configuration file */
#define DISCLAIMFILE "disclaim.txt"
#define FROMDSCLFILE "fromdscl.txt"
#define POP3CONF "pop3.cfg"
#define HELPFILE "help.txt"
#define ABUSEFILE "abuse.txt"
#define REPLYFILE "reply.txt"
#define USAGEFILE "usage.txt"
#define USAGELOG "usage.log"
#define BLOCKFILE "blocked.txt"
#define ADMKEYFILE "adminkey.txt"
#define KEYFILE "key.txt"
#define PGPKEY "pgpkey.txt"
#define DSAPARAMS "dsaparam.mix"
#define DHPARAMS "dhparam.mix"
#define MIXRAND "mixrand.bin"
#define SECRING "secring.mix"
#define PUBRING "pubring.mix"
#define IDLOG "id.log"
#define STATS "stats.log"
/* To enable multiple dest.blk files, edit the following line. */
/* Filenames must be seperated by one space.                   */
#define DESTBLOCK "dest.blk rab.blk"
#define DESTALLOW "dest.alw"
#define SOURCEBLOCK "source.blk"
#define HDRFILTER "header.blk"
#define REGULAR "time.log"
#define POOL "pool"		/* remailer pool subdirectory */
#define TYPE1LIST "rlist.txt"
#define TYPE2REL "mlist.txt"
#ifdef SHORTNAMES
#define TYPE2LIST "type2.lis"
#else
#define TYPE2LIST "type2.list"
#endif

#define PGPREMPUBRING "pubring.pgp"
#define PGPREMPUBASC "pubring.asc"
#define PGPREMSECRING "secring.pgp"
#define NYMSECRING "nymsec.pgp"
#define NYMDB "secrets.mix"

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
extern long PACKETEXP;
extern long IDEXP;
extern int VERBOSE;
extern long SENDPOOLTIME;
extern int NUMCOPIES;
extern char CHAIN[];
extern int DISTANCE;
extern int MINREL;
extern int RELFINAL;
extern long MAXLAT;
extern char PGPPUBRING[];
extern char PGPSECRING[];
extern long POP3TIME;
extern int POP3DEL;
extern int POP3SIZELIMIT;
extern char MAILBOX[];
extern char MAILABUSE[];
extern char MAILBLOCK[];
extern char MAILUSAGE[];
extern char MAILANON[];
extern char MAILERROR[];
extern char MAILBOUNCE[];

#endif
