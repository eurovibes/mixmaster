/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Mixmaster initialization, configuration
   $Id: mix.c,v 1.16 2002/08/25 07:47:23 weaselp Exp $ */


#include "mix3.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef POSIX
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/utsname.h>
#else
#include <io.h>
#include <direct.h>
#endif
#ifdef WIN32
#include <windows.h>
#endif
#include <assert.h>
#include "menu.h"

int buf_vappendf(BUFFER *b, char *fmt, va_list args);

/** filenames ************************************************************/
char MIXCONF[PATHMAX];
char DISCLAIMFILE[PATHMAX];
char FROMDSCLFILE[PATHMAX];
char MSGFOOTERFILE[PATHMAX];
char POP3CONF[PATHMAX];
char HELPFILE[PATHMAX];
char ABUSEFILE[PATHMAX];
char REPLYFILE[PATHMAX];
char USAGEFILE[PATHMAX];
char USAGELOG[PATHMAX];
char BLOCKFILE[PATHMAX];
char ADMKEYFILE[PATHMAX];
char KEYFILE[PATHMAX];
char PGPKEY[PATHMAX];
char DSAPARAMS[PATHMAX];
char DHPARAMS[PATHMAX];
char MIXRAND[PATHMAX];
char SECRING[PATHMAX];
char PUBRING[PATHMAX];
char IDLOG[PATHMAX];
char STATS[PATHMAX];
char DESTBLOCK[PATHMAX];
char DESTALLOW[PATHMAX];
char SOURCEBLOCK[PATHMAX];
char HDRFILTER[PATHMAX];
char REGULAR[PATHMAX];
char POOL[PATHMAX];
char TYPE1LIST[PATHMAX];
char TYPE2REL[PATHMAX];
char TYPE2LIST[PATHMAX];

char PGPREMPUBRING[PATHMAX];
char PGPREMPUBASC[PATHMAX];
char PGPREMSECRING[PATHMAX];
char NYMSECRING[PATHMAX];
char NYMDB[PATHMAX];


/** config ***************************************************************/

char MIXDIR[PATHMAX];
char POOLDIR[PATHMAX];

/* programs */
char SENDMAIL[LINELEN];
char SENDANONMAIL[LINELEN];
char NEWS[LINELEN];
char TYPE1[LINELEN];

/* addresses */
char MAILtoNEWS[LINELEN];
char REMAILERNAME[LINELEN];
char ANONNAME[LINELEN];
char REMAILERADDR[LINELEN];
char ANONADDR[LINELEN];
char COMPLAINTS[LINELEN];
int AUTOREPLY;
char SMTPRELAY[LINELEN];

#ifdef USE_SOCK
char HELONAME[LINELEN];
char ENVFROM[LINELEN];
int POP3DEL;
int POP3SIZELIMIT;
long POP3TIME;

#endif

char SHORTNAME[LINELEN];

/* remailer configuration */
int REMAIL;
int MIX;
int PGP;
int UNENCRYPTED;
int REMIX;
int REPGP;

int POOLSIZE;
int RATE;
int MIDDLEMAN;
int AUTOBLOCK;
char FORWARDTO[LINELEN];
int SIZELIMIT;		/* maximal size of remailed messages */
int INFLATEMAX;		/* maximal size of Inflate: padding */
int MAXRANDHOPS;
int BINFILTER;		/* filter binary attachments? */
int LISTSUPPORTED;		/* list supported remailers in remailer-conf reply? */
long PACKETEXP;	/* Expiration time for old packets */
long IDEXP;	/* 0 = no ID log !! */
long SENDPOOLTIME;	/* frequency for sending pool messages */

char ERRLOG[LINELEN];
char ADDRESS[LINELEN];
char NAME[LINELEN];

char ORGANIZATION[LINELEN];
char MID[LINELEN];

/* client config */
int NUMCOPIES;
char CHAIN[LINELEN];
int VERBOSE;
int DISTANCE;
int MINREL;
int RELFINAL;
long MAXLAT;
char PGPPUBRING[PATHMAX];
char PGPSECRING[PATHMAX];
char PASSPHRASE[LINELEN];
char MAILIN[PATHMAX];
char MAILBOX[PATHMAX];
char MAILABUSE[PATHMAX];
char MAILBLOCK[PATHMAX];
char MAILUSAGE[PATHMAX];
char MAILANON[PATHMAX];
char MAILERROR[PATHMAX];
char MAILBOUNCE[PATHMAX];

static int rereadconfig = 0;
static int terminatedaemon = 0;

#if defined(S_IFDIR) && !defined(S_ISDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

static int mixdir(char *d, int create)
{
  int err;
  struct stat buf;

  if (d != MIXDIR)
    strncpy(MIXDIR, d, PATHMAX);
  if (MIXDIR[strlen(MIXDIR) - 1] == DIRSEP)
    MIXDIR[strlen(MIXDIR) - 1] = '\0';
  err = stat(MIXDIR, &buf);
  if (err == -1) {
    if (create) {
#ifndef POSIX
      err = mkdir(MIXDIR);
#else
      err = mkdir(MIXDIR, S_IRWXU);
#endif
      if (err == 0)
	errlog(NOTICE, "Creating directory %s.\n", MIXDIR);
    } else
      err = 1;
  } else if (!S_ISDIR(buf.st_mode))
    err = -1;
  if (err == 0)
    strcatn(MIXDIR, DIRSEPSTR, PATHMAX);
  return (err);
}

void whoami(char *addr, char *defaultname)
{
  char *p = NULL;

#if defined(HAVE_GETDOMAINNAME) || (defined(HAVE_GETHOSTNAME) && ! defined(HAVE_UNAME))
  char line[LINELEN];

#endif
#ifdef HAVE_UNAME
  struct utsname uts;

#endif
#ifdef POSIX
  p = getlogin();
#endif
  if (p == NULL)
    strcpy(addr, defaultname);
  else
    strncpy(addr, p, LINELEN);

  strcatn(addr, "@", LINELEN);
#ifdef HAVE_UNAME
  if (uname(&uts) != -1)
    strcatn(addr, uts.nodename, LINELEN);
#elif defined(HAVE_GETHOSTNAME)
  if (gethostname(line, LINELEN) == 0)
    strcatn(addr, line, LINELEN);
#endif
  if (addr[strlen(addr) - 1] == '@')
    strcatn(addr, SHORTNAME, LINELEN);

  if (strchr(strchr(addr, '@'), '.') == NULL) {
#ifdef HAVE_GETDOMAINNAME
    if (getdomainname(line, LINELEN) == 0 && !streq(line, "(none)")) {
      strcatn(addr, ".", LINELEN);
      strcatn(addr, line, LINELEN);
    }
#endif
  }
}

#define read_conf(t) readconfline(line, #t, sizeof(#t)-1, t)

static int readconfline(char *line, char *name, int namelen, char *var)
{
  if (strncmp(line, name, namelen) == 0 &&
      (isspace(line[namelen]) || line[namelen] == '=')) {
    line += namelen;
    if (*line == '=')
      line++;
    while (isspace(*line))
      line++;
    if (line[0] == '\n' || line[0] == '\0')	/* leave default */
      return (1);
    strncpy(var, line, LINELEN);
    if (var[strlen(var) - 1] == '\n')
      var[strlen(var) - 1] = '\0';
    return (1);
  } else
    return (0);
}

#define read_conf_i(t) readiconfline(line, #t, sizeof(#t)-1, &t)

static int readiconfline(char *line, char *name, int namelen, int *var)
{
  if (strncmp(line, name, namelen) == 0 &&
      (isspace(line[namelen]) || line[namelen] == '=')) {
    line += namelen;
    if (*line == '=')
      line++;
    while (isspace(*line))
      line++;
    if (line[0] == '\n' || line[0] == '\0')	/* leave default */
      return (1);
    switch (tolower(line[0])) {
    case 'n':
      *var = 0;
      break;
    case 'y':
      *var = 1;
      break;
    case 'x':
      *var = 2;
      break;
    default:
      sscanf(line, "%d", var);
    }
    return (1);
  } else
    return (0);
}

#define read_conf_t(t) readtconfline(line, #t, sizeof(#t)-1, &t)

static int readtconfline(char *line, char *name, int namelen, long *var)
{
  char *linenext;
  int mod = 0;
  long l = 0;
  long n;

  if (strncmp(line, name, namelen) == 0 &&
      (isspace(line[namelen]) || line[namelen] == '=')) {
    line += namelen;
    if (*line == '=')
      line++;
    for (;; line++) {
      n = strtol(line, &linenext, 10);
      if (linenext == line)
	break;
      line = linenext;
      mod = 1;
      assert(line != NULL);
      while (isspace(*line))
	line++;
      switch (tolower(*line)) {
      case 'd':
	l += 24 * 60 * 60 * n;
	break;
      case 'm':
	l += 60 * n;
	break;
      case 'h':
      default:
	l += 60 * 60 * n;
	break;
      }
    }
    if (mod)
      *var = l;
    return (1);
  } else
    return (0);
}

void mix_setdefaults()
{
#define strnncpy(a,b) strncpy(a, b, sizeof(a)); a[sizeof(a)-1] = '\0'

	strnncpy(MIXCONF      , DEFAULT_MIXCONF);
	strnncpy(DISCLAIMFILE , DEFAULT_DISCLAIMFILE);
	strnncpy(FROMDSCLFILE , DEFAULT_FROMDSCLFILE);
	strnncpy(MSGFOOTERFILE, DEFAULT_MSGFOOTERFILE);
	strnncpy(POP3CONF     , DEFAULT_POP3CONF);
	strnncpy(HELPFILE     , DEFAULT_HELPFILE);
	strnncpy(ABUSEFILE    , DEFAULT_ABUSEFILE);
	strnncpy(REPLYFILE    , DEFAULT_REPLYFILE);
	strnncpy(USAGEFILE    , DEFAULT_USAGEFILE);
	strnncpy(USAGELOG     , DEFAULT_USAGELOG);
	strnncpy(BLOCKFILE    , DEFAULT_BLOCKFILE);
	strnncpy(ADMKEYFILE   , DEFAULT_ADMKEYFILE);
	strnncpy(KEYFILE      , DEFAULT_KEYFILE);
	strnncpy(PGPKEY       , DEFAULT_PGPKEY);
	strnncpy(DSAPARAMS    , DEFAULT_DSAPARAMS);
	strnncpy(DHPARAMS     , DEFAULT_DHPARAMS);
	strnncpy(MIXRAND      , DEFAULT_MIXRAND);
	strnncpy(SECRING      , DEFAULT_SECRING);
	strnncpy(PUBRING      , DEFAULT_PUBRING);
	strnncpy(IDLOG        , DEFAULT_IDLOG);
	strnncpy(STATS        , DEFAULT_STATS);
	strnncpy(DESTBLOCK    , DEFAULT_DESTBLOCK);
	strnncpy(DESTALLOW    , DEFAULT_DESTALLOW);
	strnncpy(SOURCEBLOCK  , DEFAULT_SOURCEBLOCK);
	strnncpy(HDRFILTER    , DEFAULT_HDRFILTER);
	strnncpy(REGULAR      , DEFAULT_REGULAR);
	strnncpy(POOL         , DEFAULT_POOL);
	strnncpy(TYPE1LIST    , DEFAULT_TYPE1LIST);
	strnncpy(TYPE2REL     , DEFAULT_TYPE2REL);
	strnncpy(TYPE2LIST    , DEFAULT_TYPE2LIST);

	strnncpy(PGPREMPUBRING, DEFAULT_PGPREMPUBRING);
	strnncpy(PGPREMPUBASC , DEFAULT_PGPREMPUBASC);
	strnncpy(PGPREMSECRING, DEFAULT_PGPREMSECRING);
	strnncpy(NYMSECRING   , DEFAULT_NYMSECRING);
	strnncpy(NYMDB        , DEFAULT_NYMDB);


	strnncpy(MIXDIR       , "");
	strnncpy(POOLDIR      , "");

/* programs */
#ifdef WIN32
	strnncpy(SENDMAIL     , "outfile");
#else
	strnncpy(SENDMAIL     , "/usr/lib/sendmail -t");
#endif
	strnncpy(SENDANONMAIL , "");
	strnncpy(NEWS         , "");
	strnncpy(TYPE1        , "");

/* addresses */
	strnncpy(MAILtoNEWS   , "mail2news@anon.lcs.mit.edu");
	strnncpy(REMAILERNAME , "Anonymous Remailer");
	strnncpy(ANONNAME     , "Anonymous");
	strnncpy(REMAILERADDR , "");
	strnncpy(ANONADDR     , "");
	strnncpy(COMPLAINTS   , "");
	strnncpy(SMTPRELAY    , "");
	AUTOREPLY             = 0;

#ifdef USE_SOCK
  	strnncpy(HELONAME     , "");
	strnncpy(ENVFROM      , "");
	POP3DEL               = 0;
	POP3SIZELIMIT         = 0;
	POP3TIME              = 60 * 60;

#endif

	strnncpy(SHORTNAME    , "");

/* 	configuration */
	REMAIL        = 0;
	MIX           = 1;
	PGP           = 1;
	UNENCRYPTED   = 0;
	REMIX         = 1;
	REPGP         = 1;

	POOLSIZE      = 0;
	RATE          = 100;
	MIDDLEMAN     = 0;
	AUTOBLOCK     = 1;
	strnncpy(FORWARDTO, "*");
	SIZELIMIT     = 0;		/* maximal size of remailed messages */
	INFLATEMAX    = 50;		/* maximal size of Inflate: padding */
	MAXRANDHOPS   = 20;
	BINFILTER     = 0;		/* filter binary attachments? */
	LISTSUPPORTED = 1;		/* list supported remailers in remailer-conf reply? */
	PACKETEXP     = 7 * SECONDSPERDAY;	/* Expiration time for old packets */
	IDEXP         = 7 * SECONDSPERDAY;	/* 0 = no ID log !! */
	SENDPOOLTIME  = 60 * 60;	/* frequency for sending pool messages */

	strnncpy(ERRLOG      , "");
	strnncpy(ADDRESS     , "");
	strnncpy(NAME        , "");

	strnncpy(ORGANIZATION, "Anonymous Posting Service");
	strnncpy(MID         , "y");

/* client config */
	NUMCOPIES = 1;
	strnncpy(CHAIN, "*,*,*,*");
	VERBOSE = 2;
	DISTANCE = 2;
	MINREL = 98;
	RELFINAL = 99;
	MAXLAT = 36 * 60 * 60;
	strnncpy(PGPPUBRING, "");
	strnncpy(PGPSECRING, "");
#ifdef COMPILEDPASS
	strnncpy(PASSPHRASE, COMPILEDPASS);
#else
	strnncpy(PASSPHRASE, "");
#endif
	strnncpy(MAILIN    , "");
	strnncpy(MAILBOX   , "mbox");
	strnncpy(MAILABUSE , "");
	strnncpy(MAILBLOCK , "");
#ifdef WIN32
	strnncpy(MAILUSAGE , "nul:");
	strnncpy(MAILANON  , "nul:");
	strnncpy(MAILERROR , "nul:");
#else
	strnncpy(MAILUSAGE , "/dev/null");
	strnncpy(MAILANON  , "/dev/null");
	strnncpy(MAILERROR , "/dev/null");
#endif
	strnncpy(MAILBOUNCE, "");
}

int mix_configline(char *line)
{
  return (read_conf(ADDRESS) || read_conf(NAME) ||
	  read_conf(SHORTNAME) || read_conf(REMAILERADDR) ||
	  read_conf(ANONADDR) || read_conf(REMAILERNAME) ||
	  read_conf(ANONNAME) || read_conf(COMPLAINTS) ||
	  read_conf_i(AUTOREPLY) || read_conf(SMTPRELAY) ||
#ifdef USE_SOCK
	  read_conf(HELONAME) || read_conf(ENVFROM) ||
#endif
	  read_conf(SENDMAIL) || read_conf(SENDANONMAIL) ||
	  read_conf_i(REMAIL) || read_conf_i(MIX) ||
	  read_conf_i(PGP) || read_conf_i(UNENCRYPTED) ||
	  read_conf_i(REMIX) || read_conf(NEWS) ||
	  read_conf_i(REPGP) ||
	  read_conf(MAILtoNEWS) || read_conf(ERRLOG) ||
	  read_conf(ORGANIZATION) || read_conf(MID) ||
	  read_conf(TYPE1) || read_conf_i(POOLSIZE) ||
	  read_conf_i(RATE) || read_conf_i(MIDDLEMAN) ||
	  read_conf_i(AUTOBLOCK) || read_conf(FORWARDTO) ||
	  read_conf_i(SIZELIMIT) || read_conf_i(INFLATEMAX) ||
	  read_conf_i(MAXRANDHOPS) || read_conf_i(BINFILTER) ||
	  read_conf_i(LISTSUPPORTED) ||
	  read_conf_t(PACKETEXP) || read_conf_t(IDEXP) ||
	  read_conf_t(SENDPOOLTIME) || read_conf_i(NUMCOPIES) ||
	  read_conf(CHAIN) || read_conf_i(VERBOSE) ||
	  read_conf_i(DISTANCE) || read_conf_i(MINREL) ||
	  read_conf_i(RELFINAL) || read_conf_t(MAXLAT) ||
	  read_conf(PGPPUBRING) || read_conf(PGPSECRING) ||
	  read_conf(PASSPHRASE) ||
#ifdef USE_SOCK
	  read_conf_i(POP3DEL) || read_conf_i(POP3SIZELIMIT) ||
	  read_conf_t(POP3TIME) ||
#endif
	  read_conf(MAILBOX) || read_conf(MAILABUSE) ||
	  read_conf(MAILBLOCK) || read_conf(MAILUSAGE) ||
	  read_conf(MAILANON) || read_conf(MAILERROR) ||
	  read_conf(MAILBOUNCE) || read_conf(MAILIN) ||

	  read_conf(DISCLAIMFILE) || read_conf(FROMDSCLFILE) ||
	  read_conf(MSGFOOTERFILE) ||
	  read_conf(POP3CONF) || read_conf(HELPFILE) ||
	  read_conf(ABUSEFILE) || read_conf(REPLYFILE) ||
	  read_conf(USAGEFILE) || read_conf(USAGELOG) ||
	  read_conf(BLOCKFILE) || read_conf(ADMKEYFILE) ||
	  read_conf(KEYFILE) || read_conf(PGPKEY) ||
	  read_conf(DSAPARAMS) || read_conf(DHPARAMS) ||
	  read_conf(MIXRAND) || read_conf(SECRING) ||
	  read_conf(PUBRING) || read_conf(IDLOG) ||
	  read_conf(STATS) || read_conf(DESTBLOCK) ||
	  read_conf(DESTALLOW) || read_conf(SOURCEBLOCK) ||
	  read_conf(HDRFILTER) || read_conf(REGULAR) ||
	  read_conf(POOL) || read_conf(TYPE1LIST) ||
	  read_conf(TYPE2REL) || read_conf(TYPE2LIST) ||
	  read_conf(PGPREMPUBRING) || read_conf(PGPREMPUBASC) ||
	  read_conf(PGPREMSECRING) || read_conf(NYMSECRING) ||
	  read_conf(NYMDB) );
}

static int mix_config(void)
{
  char *d;
  FILE *f;
  char line[PATHMAX];
  int err = -1;
#ifdef POSIX
  struct passwd *pw;
#endif
  struct stat buf;
#ifdef HAVE_UNAME
  struct utsname uts;
#endif
#ifdef WIN32
  HKEY regsw, reg, regpgp;
  DWORD type, len;
  int rkey = 0;
#endif

  mix_setdefaults();

#ifdef POSIX
  pw = getpwuid(getuid());
#endif

#ifdef WIN32
  RegOpenKeyEx(HKEY_CURRENT_USER, "Software", 0, KEY_ALL_ACCESS, &regsw);
  len=sizeof(line);
  if (err == -1 &&
      RegOpenKeyEx(regsw, "Mixmaster", 0, KEY_QUERY_VALUE, &reg) == 0) {
    if (RegQueryValueEx(reg, "MixDir", 0, &type, line, &len) == 0)
      err = mixdir(line, 1);
    RegCloseKey(reg);
  }
#endif

  if (err == -1 && (d = getenv("MIXPATH")) != NULL)
    err = mixdir(d, 1);

#ifdef SPOOL
  if (err == -1 && strlen(SPOOL) > 0)
    err = mixdir(SPOOL, 0);
#endif

#ifdef POSIX
  if (err == -1 && pw != NULL) {
    strncpy(line, pw->pw_dir, PATHMAX);
    line[PATHMAX-1] = '\0';
    if (line[strlen(line) - 1] != DIRSEP)
      strcatn(line, DIRSEPSTR, PATHMAX);
    strcatn(line, HOMEMIXDIR, PATHMAX);
    err = mixdir(line, 1);
  }
#endif

  if (err == -1) {
    getcwd(MIXDIR, PATHMAX);
    mixdir(MIXDIR, 0);
  }

  mixfile(POOLDIR, POOL);
  if (POOLDIR[strlen(POOLDIR) - 1] == DIRSEP)
    POOLDIR[strlen(POOLDIR) - 1] = '\0';
  if (stat(POOLDIR, &buf) != 0)
    if
#ifndef POSIX
      (mkdir(POOLDIR) != 0)
#else
      (mkdir(POOLDIR, S_IRWXU) == -1)
#endif
      strncpy(POOLDIR, MIXDIR, PATHMAX);

#ifdef GLOBALMIXCONF
  f = mix_openfile(GLOBALMIXCONF, "r");
  if (f != NULL) {
    while (fgets(line, LINELEN, f) != NULL)
      if (line[0] > ' ' && line[0] != '#')
	mix_configline(line);
    fclose(f);
  }
#endif
  f = mix_openfile(MIXCONF, "r");
  if (f != NULL) {
    while (fgets(line, LINELEN, f) != NULL)
      if (line[0] > ' ' && line[0] != '#')
	mix_configline(line);
    fclose(f);
  }
  if (IDEXP > 0 && IDEXP < 5 * SECONDSPERDAY)
    IDEXP = 5 * SECONDSPERDAY;
  if (MAXRANDHOPS > 20)
    MAXRANDHOPS = 20;

  if (strchr(SHORTNAME, '.'))
    *strchr(SHORTNAME, '.') = '\0';
  if (strchr(SHORTNAME, ' '))
    *strchr(SHORTNAME, ' ') = '\0';
#ifdef HAVE_UNAME
  if (SHORTNAME[0] == '\0' && uname(&uts) != -1)
    strncpy(SHORTNAME, uts.nodename, LINELEN);
#elif defined(HAVE_GETHOSTNAME)
  if (SHORTNAME[0] == '\0')
    gethostname(SHORTNAME, LINELEN);
#endif
  if (SHORTNAME[0] == '\0')
    strcpy(SHORTNAME, "unknown");

  if (ADDRESS[0] == '\0')
    whoami(ADDRESS, "user");

#ifdef HAVE_GECOS
  if (NAME[0] == '\0' && pw != NULL)
    strcatn(NAME, pw->pw_gecos, sizeof(NAME));
#endif

  if (REMAILERADDR[0] == '\0')
    strncpy(REMAILERADDR, ADDRESS, LINELEN);

  if (COMPLAINTS[0] == '\0')
    strncpy(COMPLAINTS, REMAILERADDR, LINELEN);

  if (strchr(REMAILERNAME, '@') == NULL) {
    strcatn(REMAILERNAME, " <", LINELEN);
    strcatn(REMAILERNAME, REMAILERADDR, LINELEN);
    strcatn(REMAILERNAME, ">", LINELEN);
  }
  if (strchr(ANONNAME, '@') == NULL && ANONADDR[0] != '\0') {
    strcatn(ANONNAME, " <", LINELEN);
    strcatn(ANONNAME, ANONADDR, LINELEN);
    strcatn(ANONNAME, ">", LINELEN);
  }
  if (strchr(ANONNAME, '@') == NULL) {
    strcatn(ANONNAME, " <", LINELEN);
    strcatn(ANONNAME, REMAILERADDR, LINELEN);
    strcatn(ANONNAME, ">", LINELEN);
  }
#ifndef USE_PGP
  if (TYPE1[0] == '\0')
    PGP = 0;
#endif
#ifndef USE_RSA
  MIX = 0;
#endif

#ifdef WIN32
  if (RegOpenKeyEx(regsw, "PGP", 0, KEY_ALL_ACCESS, &regpgp) == 0) 
    rkey++;
  if (rkey && RegOpenKeyEx(regpgp, "PGPlib", 0, KEY_QUERY_VALUE, &reg) == 0)
    rkey++;
  if (PGPPUBRING[0] == '\0' && rkey == 2) {
    len = PATHMAX;
    RegQueryValueEx(reg, "PubRing", 0, &type, PGPPUBRING, &len);
  }
  if (PGPSECRING[0] == '\0' && rkey == 2) {
    len = PATHMAX;
    RegQueryValueEx(reg, "SecRing", 0, &type, PGPSECRING, &len);
  }
  if (rkey == 2)
    RegCloseKey(reg);
  if (rkey)
    RegCloseKey(regpgp);
  RegCloseKey(regsw);
#endif

  if (PGPPUBRING[0] == '\0') {
    char *d;

    if ((d = getenv("HOME")) != NULL) {
      strcpy(PGPPUBRING, d);
      strcatn(PGPPUBRING, "/.pgp/", PATHMAX);
    }
    strcatn(PGPPUBRING, "pubring.pkr", PATHMAX);
    if (stat(PGPPUBRING, &buf) == -1)
      strcpy(strrchr(PGPPUBRING, '.'), ".pgp");
  }
  if (PGPSECRING[0] == '\0') {
    char *d;

    if ((d = getenv("HOME")) != NULL) {
      strcpy(PGPSECRING, d);
      strcatn(PGPSECRING, "/.pgp/", PATHMAX);
    }
    strcatn(PGPSECRING, "secring.skr", PATHMAX);
    if (stat(PGPSECRING, &buf) == -1)
      strcpy(strrchr(PGPSECRING, '.'), ".pgp");
  }
  if (streq(NEWS, "mail-to-news"))
    strncpy(NEWS, MAILtoNEWS, sizeof(NEWS));

  if (f == NULL) {
#ifndef GLOBALMIXCONF
    /* Only write the config file in non systemwide installation */
    f = mix_openfile(MIXCONF, "w");
    if (f == NULL)
      errlog(WARNING, "Can't open %s%s!\n", MIXDIR, MIXCONF);
    else {
      fprintf(f, "# mix.cfg - mixmaster configuration file\n");
      fprintf(f, "NAME	        %s\n", NAME);
      fprintf(f, "ADDRESS	        %s\n", ADDRESS);
      fprintf(f, "\n# edit to set up a remailer:\n");
      fprintf(f, "REMAIL          n\n");
      fprintf(f, "SHORTNAME	%s\n", SHORTNAME);
      fprintf(f, "REMAILERADDR	%s\n", REMAILERADDR);
      fprintf(f, "COMPLAINTS	%s\n", COMPLAINTS);
      fclose(f);
    }
#endif
    REMAIL = 0;
  }

  return (0);
}

/** Library initialization: ******************************************/

static int initialized = 0;

int mix_init(char *mixdir)
{
  if (!initialized) {
    if (mixdir)
      strncpy(MIXDIR, mixdir, LINELEN);
    mix_config();
#if defined(USE_SOCK) && defined(WIN32)
    sock_init();
#endif
    /* atexit (mix_exit); */
    initialized = 1;
  }

  if (rnd_init() == -1)
    rnd_seed();
  return(0);
}

void mix_exit(void)
{
  if (!initialized)
    return;
  rnd_final();
#if defined(USE_SOCK) && defined(WIN32)
  sock_exit();
#endif
  initialized=0;
}

int mix_regular(int force)
{
  FILE *f;
  long now, tpool = 0, tpop3 = 0, tdaily = 0;
  int ret = 0;

  mix_init(NULL);
  now = time(NULL);

  f = mix_openfile(REGULAR, "r+");
  if (f != NULL) {
    lock(f);
    fscanf(f, "%ld %ld %ld", &tpool, &tpop3, &tdaily);
    if (now - tpool >= SENDPOOLTIME)
      force |= FORCE_POOL;
#ifdef USE_SOCK
    if (now - tpop3 >= POP3TIME)
      force |= FORCE_POP3;
#endif
    if (now - tdaily >= SECONDSPERDAY)
      force |= FORCE_DAILY;
    if (force & FORCE_POOL)
      tpool = now;
    if (force & FORCE_POP3)
      tpop3 = now;
    if (force & FORCE_DAILY)
      tdaily = now;
    rewind(f);
    fprintf(f, "%ld %ld %ld\n", tpool, tpop3, tdaily);
    unlock(f);
    fclose(f);
  } else {
    force = FORCE_POOL | FORCE_POP3 | FORCE_DAILY;
    f = mix_openfile(REGULAR, "w+");
    if (f != NULL) {
      lock(f);
      fprintf(f, "%ld %ld %ld\n", now, now, now);
      unlock(f);
      fclose(f);
    } else
      errlog(ERRORMSG, "Can't create %s!\n", REGULAR);
  }

  if (force & FORCE_DAILY)
    mix_daily(), ret = 1;
#ifdef USE_SOCK
  if (force & FORCE_POP3)
    pop3get();
#endif
  if (force & FORCE_POOL)
    ret = pool_send();

  return (ret);
}

int mix_daily(void)
{
  idexp();
  pool_packetexp();
  stats(NULL);
  keymgt(0);
  return (0);
}

/** Handle signals SIGHUP, SIGINT, and SIGTERM
    This signal handler gets called if the daemon
    process receives one of SIGHUP, SIGINT, or SIGTERM.
    It then sets either rereadconfig of terminatedaemon
    to true depending on the signal received.

    @author	PP
    @return	nothing
 */
void sighandler(int signal) {
  if (signal == SIGHUP)
    rereadconfig = 1;
  else if (signal == SIGINT || signal == SIGTERM)
    terminatedaemon = 1;
};

/** Set the signal handler for SIGHUP, SIGINT and SIGTERM
    This function registers signal handlers so that
    we can react on signals send by the user in daemon
    mode. SIGHUP will instruct mixmaster to reload its
    configuration while SIGINT and SIGTERM will instruct
    it to shut down. Mixmaster will finish the current
    pool run before it terminates.

    @param restart  Whether or not system calls should be
                    restarted. Usually we want this, the
		    only excetion is the sleep() in the
		    daemon mail loop.
    @author         PP
    @return         -1 if calling sigaction failed, 0 on
                    no error
  */
int setsignalhandler(int restart)
{
#ifdef POSIX
  struct sigaction hdl;
  int err = 0;

  memset(&hdl, 0, sizeof(hdl));
  hdl.sa_handler = sighandler;
  hdl.sa_flags = restart ? SA_RESTART : 0;
  
  if (sigaction(SIGHUP, &hdl, NULL))
    err = -1;
  if (sigaction(SIGINT, &hdl, NULL))
    err = -1;
  if (sigaction(SIGTERM, &hdl, NULL))
    err = -1;
  return (err);
#else /* POSIX */
  return(0);
#endif /* POSIX */
}

#ifdef WIN32
/* Try to detect if we are the service or not... 
   seems there is no easy reliable way        */
int is_nt_service(void)
{ 
    static int issvc = -1;
#ifdef WIN32SERVICE
    STARTUPINFO StartupInfo;
    OSVERSIONINFO VersionInfo;
    DWORD dwsize;

    if (issvc != -1)    /* do it only once */
        return issvc;

    VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
    if (GetVersionEx(&VersionInfo))
        if (VersionInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
            return issvc = 0; /* not NT - not the service */

    if (!GetConsoleTitle(&VersionInfo,sizeof(VersionInfo))) 
    /* reuse VersionInfo to save memory */
        return issvc = 1; /* have no console - we are the service probably */

    GetStartupInfo(&StartupInfo);
    if (StartupInfo.lpDesktop[0] == 0)
        return issvc = 1; /* have no desktop - we are the service probably */

    if (_fileno(stdin) == -1 && _fileno(stdout) == -1 && _fileno(stderr) == -1)
        return issvc = 1; /* have no stdin,stderr,stdout - probably service */
#endif // WIN32SERVICE

    return issvc = 0; // assume not the service
} // is_nt_service

HANDLE hMustTerminate = NULL;
void set_nt_exit_event(HANDLE h_svc_exit_event)
{
    hMustTerminate = h_svc_exit_event;
} // set_nt_exit_event

#endif // WIN32

int mix_daemon(void)
{
  long t, slept;
  t = SENDPOOLTIME;
#ifdef USE_SOCK
  if (POP3TIME < t)
    t = POP3TIME;
#endif
  slept = t;

  setsignalhandler(1); /* set signal handlers and restart any interrupted system calls */
  for(;;) {
    if (terminatedaemon)
      exit(0);
    if (rereadconfig) {
      rereadconfig = 0;
      mix_config();
      t = SENDPOOLTIME;
#ifdef USE_SOCK
      if (POP3TIME < t)
	t = POP3TIME;
#endif
    }
    if (slept >= t) {
      mix_regular(0);
      slept = 0;
    }

    if (!terminatedaemon && !rereadconfig) {
      setsignalhandler(0); /* set signal handlers;  don't restart system calls */
#ifdef WIN32
#ifdef WIN32SERVICE
      if (hMustTerminate) {
	if (WaitForSingleObject(hMustTerminate, t * 1000) == WAIT_OBJECT_0) {
	  CloseHandle(hMustTerminate);
	  return 0;
	}
      } else
#endif
      Sleep(t * 1000);
#else
      slept += (t - slept) - sleep(t - slept);
#endif
      setsignalhandler(1); /* set signal handlers and restart any interrupted system calls */
    }
  }
}

/** error ***************************************************************/

void errlog(int type, char *fmt,...)
{
  va_list args;
  BUFFER *msg;
  FILE *e = NULL;
  time_t t;
  struct tm *tc;
  char line[LINELEN];
  int p;
  char err[6][8] =
  {"", "Error", "Warning", "Notice", "Info", "Info"};

  if ((VERBOSE == 0 && type != ERRORMSG) || (type == LOG && VERBOSE < 2)
      || (type == DEBUGINFO && VERBOSE < 3))
    return;

  t = time(NULL);
  tc = localtime(&t);
  strftime(line, LINELEN, "[%Y-%m-%d %H:%M:%S] ", tc);

  msg = buf_new();
  buf_appends(msg, line);
  p = msg->length;
  buf_appendf(msg, "%s: ", err[type]);
  va_start(args, fmt);
  buf_vappendf(msg, fmt, args);
  va_end(args);

  if (streq(ERRLOG, "stdout"))
    e = stdout;
  else if (streq(ERRLOG, "stderr"))
    e = stderr;

  if (e == NULL && (ERRLOG[0] == '\0' ||
		    (e = mix_openfile(ERRLOG, "a")) == NULL))
    mix_status("%s", msg->data + p);
  else {
    buf_write(msg, e);
    if (e != stderr && e != stdout) {
      fclose(e);
      /* duplicate the error message on screen */
      mix_status("%s", msg->data + p);
    }
  }
  buf_free(msg);
}

static char statusline[BUFSIZE] = "";

void mix_status(char *fmt,...)
{
  va_list args;

  if (fmt != NULL) {
    va_start(args, fmt);
#ifdef _MSC
    _vsnprintf(statusline, sizeof(statusline) - 1, fmt, args);
#else
    vsnprintf(statusline, sizeof(statusline) - 1, fmt, args);
#endif
    va_end(args);
  }
#ifdef USE_NCURSES
  if (menu_initialized) {
    cl(LINES - 2, 10);
    printw("%s", statusline);
    refresh();
  } else
#endif
  {
    fprintf(stderr, "%s", statusline);
  }
}

void mix_genericerror(void)
{
  if (streq(statusline, "") || strfind(statusline, "...") ||
      strifind(statusline, "generating"))
    mix_status("Failed!");
  else
    mix_status(NULL);
}
