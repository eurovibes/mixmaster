/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Mixmaster initialization, configuration
   $Id: mix.c,v 1.11.2.8 2002/10/11 01:19:16 weaselp Exp $ */


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
#else /* end of POSIX */
#include <io.h>
#include <direct.h>
#endif /* else if not POSIX */
#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */
#include <assert.h>
#include "menu.h"

int buf_vappendf(BUFFER *b, char *fmt, va_list args);

/** filenames ************************************************************/
char MIXCONF[PATHMAX] = DEFAULT_MIXCONF;       /* mixmaster configuration file */
char DISCLAIMFILE[PATHMAX] = DEFAULT_DISCLAIMFILE;
char FROMDSCLFILE[PATHMAX] = DEFAULT_FROMDSCLFILE;
char MSGFOOTERFILE[PATHMAX] = DEFAULT_MSGFOOTERFILE;
char POP3CONF[PATHMAX] = DEFAULT_POP3CONF;
char HELPFILE[PATHMAX] = DEFAULT_HELPFILE;
char ABUSEFILE[PATHMAX] = DEFAULT_ABUSEFILE;
char REPLYFILE[PATHMAX] = DEFAULT_REPLYFILE;
char USAGEFILE[PATHMAX] = DEFAULT_USAGEFILE;
char USAGELOG[PATHMAX] = DEFAULT_USAGELOG;
char BLOCKFILE[PATHMAX] = DEFAULT_BLOCKFILE;
char ADMKEYFILE[PATHMAX] = DEFAULT_ADMKEYFILE;
char KEYFILE[PATHMAX] = DEFAULT_KEYFILE;
char PGPKEY[PATHMAX] = DEFAULT_PGPKEY;
char DSAPARAMS[PATHMAX] = DEFAULT_DSAPARAMS;
char DHPARAMS[PATHMAX] = DEFAULT_DHPARAMS;
char MIXRAND[PATHMAX] = DEFAULT_MIXRAND;
char SECRING[PATHMAX] = DEFAULT_SECRING;
char PUBRING[PATHMAX] = DEFAULT_PUBRING;
char IDLOG[PATHMAX] = DEFAULT_IDLOG;
char STATS[PATHMAX] = DEFAULT_STATS;
/* To enable multiple dest.blk files, edit the following line. */
/* Filenames must be seperated by one space.                   */
char DESTBLOCK[PATHMAX] = DEFAULT_DESTBLOCK;
char DESTALLOW[PATHMAX] = DEFAULT_DESTALLOW;
char SOURCEBLOCK[PATHMAX] = DEFAULT_SOURCEBLOCK;
char HDRFILTER[PATHMAX] = DEFAULT_HDRFILTER;
char REGULAR[PATHMAX] = DEFAULT_REGULAR;
char POOL[PATHMAX] = DEFAULT_POOL;             /* remailer pool subdirectory */
char TYPE1LIST[PATHMAX] = DEFAULT_TYPE1LIST;
char TYPE2REL[PATHMAX] = DEFAULT_TYPE2REL;
char TYPE2LIST[PATHMAX] = DEFAULT_TYPE2LIST;
char PIDFILE[PATHMAX] = DEFAULT_PIDFILE;

char PGPREMPUBRING[PATHMAX] = DEFAULT_PGPREMPUBRING;
char PGPREMPUBASC[PATHMAX] = DEFAULT_PGPREMPUBASC;
char PGPREMSECRING[PATHMAX] = DEFAULT_PGPREMSECRING;
char NYMSECRING[PATHMAX] = DEFAULT_NYMSECRING;
char NYMDB[PATHMAX] = DEFAULT_NYMDB;


/** config ***************************************************************/

char MIXDIR[PATHMAX];
char POOLDIR[PATHMAX];

/* programs */
#ifdef WIN32
char SENDMAIL[LINELEN] = "outfile";
#else /* end of WIN32 */
char SENDMAIL[LINELEN] = "/usr/lib/sendmail -t";
#endif /* else if not WIN32 */
char SENDANONMAIL[LINELEN];
char NEWS[LINELEN];
char TYPE1[LINELEN];

/* addresses */
char MAILtoNEWS[LINELEN] = "mail2news@anon.lcs.mit.edu";
char REMAILERNAME[LINELEN] = "Anonymous Remailer";
char ANONNAME[LINELEN] = "Anonymous";
char REMAILERADDR[LINELEN];
char ANONADDR[LINELEN];
char COMPLAINTS[LINELEN];
int AUTOREPLY;
char SMTPRELAY[LINELEN];
char SMTPUSERNAME[LINELEN];
char SMTPPASSWORD[LINELEN];

#ifdef USE_SOCK
char HELONAME[LINELEN];
char ENVFROM[LINELEN];
int POP3DEL = 0;
int POP3SIZELIMIT = 0;
long POP3TIME = 60 * 60;

#endif /* USE_SOCK */

char SHORTNAME[LINELEN];

/* remailer configuration */
int REMAIL = 0;
int MIX = 1;
int PGP = 1;
int UNENCRYPTED = 0;
int REMIX = 1;
int REPGP = 1;

int POOLSIZE = 0;
int RATE = 100;
int MIDDLEMAN = 0;
int AUTOBLOCK = 1;
char FORWARDTO[LINELEN] = "*";
int SIZELIMIT = 0;		/* maximal size of remailed messages */
int INFLATEMAX = 50;		/* maximal size of Inflate: padding */
int MAXRANDHOPS = 20;
int BINFILTER = 0;		/* filter binary attachments? */
int LISTSUPPORTED = 1;                /* list supported remailers in remailer-conf reply? */
long PACKETEXP = 7 * SECONDSPERDAY;	/* Expiration time for old packets */
long IDEXP = 7 * SECONDSPERDAY;	/* 0 = no ID log !! */
long SENDPOOLTIME = 60 * 60;	/* frequency for sending pool messages */
long MAILINTIME = 5 * 60;	/* frequency for processing MAILIN mail */

char ERRLOG[LINELEN];
char ADDRESS[LINELEN];
char NAME[LINELEN];

char ORGANIZATION[LINELEN] = "Anonymous Posting Service";
char MID[LINELEN] = "y";

/* client config */
int NUMCOPIES = 1;
char CHAIN[LINELEN] = "*,*,*,*";
int VERBOSE = 2;
int DISTANCE = 2;
int MINREL = 98;
int RELFINAL = 99;
long MAXLAT = 36 * 60 * 60;
char PGPPUBRING[PATHMAX];
char PGPSECRING[PATHMAX];
#ifdef COMPILEDPASS
char PASSPHRASE[LINELEN] = COMPILEDPASS;
#else /* end of COMPILEDPASS */
char PASSPHRASE[LINELEN] = "";
#endif /* else if not COMPILEDPASS */
char MAILIN[PATHMAX] = "";
char MAILBOX[PATHMAX] = "mbox";
char MAILABUSE[PATHMAX];
char MAILBLOCK[PATHMAX];
#ifdef WIN32
char MAILUSAGE[PATHMAX] = "nul:";
char MAILANON[PATHMAX] = "nul:";
char MAILERROR[PATHMAX] = "nul:";
#else /* end of WIN32 */
char MAILUSAGE[PATHMAX] = "/dev/null";
char MAILANON[PATHMAX] = "/dev/null";
char MAILERROR[PATHMAX] = "/dev/null";
#endif /* else if not WIN32 */
char MAILBOUNCE[PATHMAX];

static int terminatedaemon = 0;

#if defined(S_IFDIR) && !defined(S_ISDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif /* defined(S_IFDIR) && !defined(S_ISDIR) */

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
#else /* end of not POSIX */
      err = mkdir(MIXDIR, S_IRWXU);
#endif /* else if POSIX */
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

#endif /* defined(HAVE_GETDOMAINNAME) || [...] */
#ifdef HAVE_UNAME
  struct utsname uts;

#endif /* HAVE_UNAME */
#ifdef POSIX
  p = getlogin();
#endif /* POSIX */
  if (p == NULL)
    strcpy(addr, defaultname);
  else
    strncpy(addr, p, LINELEN);

  strcatn(addr, "@", LINELEN);
#ifdef HAVE_UNAME
  if (uname(&uts) != -1)
    strcatn(addr, uts.nodename, LINELEN);
#elif defined(HAVE_GETHOSTNAME) /* end of HAVE_UNAME */
  if (gethostname(line, LINELEN) == 0)
    strcatn(addr, line, LINELEN);
#endif /* defined(HAVE_GETHOSTNAME) */
  if (addr[strlen(addr) - 1] == '@')
    strcatn(addr, SHORTNAME, LINELEN);

  if (strchr(strchr(addr, '@'), '.') == NULL) {
#ifdef HAVE_GETDOMAINNAME
    if (getdomainname(line, LINELEN) == 0 && !streq(line, "(none)")) {
      strcatn(addr, ".", LINELEN);
      strcatn(addr, line, LINELEN);
    }
#endif /* HAVE_GETDOMAINNAME */
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

int mix_configline(char *line)
{
  return (read_conf(ADDRESS) || read_conf(NAME) ||
	  read_conf(SHORTNAME) || read_conf(REMAILERADDR) ||
	  read_conf(ANONADDR) || read_conf(REMAILERNAME) ||
	  read_conf(ANONNAME) || read_conf(COMPLAINTS) ||
	  read_conf_i(AUTOREPLY) || read_conf(SMTPRELAY) ||
	  read_conf(SMTPUSERNAME) || read_conf(SMTPPASSWORD) ||
#ifdef USE_SOCK
	  read_conf(HELONAME) || read_conf(ENVFROM) ||
#endif /* USE_SOCK */
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
	  read_conf_t(PACKETEXP) || read_conf_t(IDEXP) ||
	read_conf_i(LISTSUPPORTED) ||
	  read_conf_t(SENDPOOLTIME) || read_conf_i(NUMCOPIES) ||
	  read_conf_t(MAILINTIME) ||
	  read_conf(CHAIN) || read_conf_i(VERBOSE) ||
	  read_conf_i(DISTANCE) || read_conf_i(MINREL) ||
	  read_conf_i(RELFINAL) || read_conf_t(MAXLAT) ||
	  read_conf(PGPPUBRING) || read_conf(PGPSECRING) ||
	  read_conf(PASSPHRASE) ||
#ifdef USE_SOCK
	  read_conf_i(POP3DEL) || read_conf_i(POP3SIZELIMIT) ||
	  read_conf_t(POP3TIME) ||
#endif /* USE_SOCK */
	  read_conf(MAILBOX) || read_conf(MAILABUSE) ||
	  read_conf(MAILBLOCK) || read_conf(MAILUSAGE) ||
	  read_conf(MAILANON) || read_conf(MAILERROR) ||
	  read_conf(MAILBOUNCE) || read_conf(MAILIN) ||

	  read_conf(DISCLAIMFILE) || read_conf(FROMDSCLFILE) ||
	  read_conf(POP3CONF) || read_conf(HELPFILE) ||
	read_conf(MSGFOOTERFILE) ||
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
	  read_conf(NYMDB) || read_conf(PIDFILE) );

}

int mix_config(void)
{
  char *d;
  FILE *f;
  char line[PATHMAX];
  int err = -1;
#ifdef POSIX
  struct passwd *pw;
#endif /* POSIX */
  struct stat buf;
#ifdef HAVE_UNAME
  struct utsname uts;
#endif /* HAVE_UNAME */
#ifdef WIN32
  HKEY regsw, reg, regpgp;
  DWORD type, len;
  int rkey = 0;
#endif /* WIN32 */

#ifdef POSIX
  pw = getpwuid(getuid());
#endif /* POSIX */

  if (MIXDIR[0])		/* if set by main() */
    err = mixdir(MIXDIR, 1);

#ifdef WIN32
  RegOpenKeyEx(HKEY_CURRENT_USER, "Software", 0, KEY_ALL_ACCESS, &regsw);
  len=sizeof(line);
  if (err == -1 &&
      RegOpenKeyEx(regsw, "Mixmaster", 0, KEY_QUERY_VALUE, &reg) == 0) {
    if (RegQueryValueEx(reg, "MixDir", 0, &type, line, &len) == 0)
      err = mixdir(line, 1);
    RegCloseKey(reg);
  }
#endif /* WIN32 */

  if (err == -1 && (d = getenv("MIXPATH")) != NULL)
    err = mixdir(d, 1);

#ifdef SPOOL
  if (err == -1 && strlen(SPOOL) > 0)
    err = mixdir(SPOOL, 0);
#endif /* SPOOL */

#ifdef POSIX
  if (err == -1 && pw != NULL) {
    strncpy(line, pw->pw_dir, PATHMAX);
    line[PATHMAX-1] = '\0';
    if (line[strlen(line) - 1] != DIRSEP)
      strcatn(line, DIRSEPSTR, PATHMAX);
    strcatn(line, HOMEMIXDIR, PATHMAX);
    err = mixdir(line, 1);
  }
#endif /* POSIX */

  if (err == -1) {
    getcwd(MIXDIR, PATHMAX);
    mixdir(MIXDIR, 0);
  }

#ifdef GLOBALMIXCONF
  f = mix_openfile(GLOBALMIXCONF, "r");
  if (f != NULL) {
    while (fgets(line, LINELEN, f) != NULL)
      if (line[0] > ' ' && line[0] != '#')
	mix_configline(line);
    fclose(f);
  }
#endif /* GLOBALMIXCONF */
  f = mix_openfile(MIXCONF, "r");
  if (f != NULL) {
    while (fgets(line, LINELEN, f) != NULL)
      if (line[0] > ' ' && line[0] != '#')
	mix_configline(line);
    fclose(f);
  }

  mixfile(POOLDIR, POOL); /* set POOLDIR after reading POOL from cfg file */
  if (POOLDIR[strlen(POOLDIR) - 1] == DIRSEP)
    POOLDIR[strlen(POOLDIR) - 1] = '\0';
  if (stat(POOLDIR, &buf) != 0)
    if
#ifndef POSIX
      (mkdir(POOLDIR) != 0)
#else /* end of not POSIX */
      (mkdir(POOLDIR, S_IRWXU) == -1)
#endif /* else if POSIX */
      strncpy(POOLDIR, MIXDIR, PATHMAX);

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
#elif defined(HAVE_GETHOSTNAME) /* end of HAVE_UNAME */
  if (SHORTNAME[0] == '\0')
    gethostname(SHORTNAME, LINELEN);
#endif /* defined(HAVE_GETHOSTNAME) */
  if (SHORTNAME[0] == '\0')
    strcpy(SHORTNAME, "unknown");

  if (ADDRESS[0] == '\0')
    whoami(ADDRESS, "user");

#ifdef HAVE_GECOS
  if (NAME[0] == '\0' && pw != NULL)
    strcatn(NAME, pw->pw_gecos, sizeof(NAME));
#endif /* HAVE_GECOS */

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
#endif /* not USE_PGP */
#ifndef USE_RSA
  MIX = 0;
#endif /* not USE_RSA */

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
#endif /* WIN32 */

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
#endif /* not GLOBALMIXCONF */
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
#endif /* defined(USE_SOCK) && defined(WIN32) */
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
#endif /* defined(USE_SOCK) && defined(WIN32) */
  initialized=0;
}

int mix_regular(int force)
{
  FILE *f;
  long now, tpool = 0, tpop3 = 0, tdaily = 0, tmailin = 0;
  int ret = 0;

  mix_init(NULL);
  now = time(NULL);

  f = mix_openfile(REGULAR, "r+");
  if (f != NULL) {
    lock(f);
    fscanf(f, "%ld %ld %ld %ld", &tpool, &tpop3, &tdaily, &tmailin);
    if (now - tpool >= SENDPOOLTIME)
      force |= FORCE_POOL | FORCE_MAILIN;
#ifdef USE_SOCK
    if (now - tpop3 >= POP3TIME)
      force |= FORCE_POP3 | FORCE_MAILIN;
#endif /* USE_SOCK */
    if (now - tdaily >= SECONDSPERDAY)
      force |= FORCE_DAILY;
    if (now - tmailin >= MAILINTIME)
      force |= FORCE_MAILIN;
    if (force & FORCE_POOL)
      tpool = now;
    if (force & FORCE_POP3)
      tpop3 = now;
    if (force & FORCE_DAILY)
      tdaily = now;
    if (force & FORCE_MAILIN)
      tmailin = now;
    rewind(f);
    fprintf(f, "%ld %ld %ld %ld\n", tpool, tpop3, tdaily, tmailin);
    unlock(f);
    fclose(f);
  } else {
    force = FORCE_POOL | FORCE_POP3 | FORCE_DAILY | FORCE_MAILIN;
    f = mix_openfile(REGULAR, "w+");
    if (f != NULL) {
      lock(f);
      fprintf(f, "%ld %ld %ld %ld\n", now, now, now, now);
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
#endif /* USE_SOCK */
  if (force & FORCE_MAILIN)
    ret = process_mailin();
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
#ifdef POSIX
void sighandler(int signal) {
  if (signal == SIGINT || signal == SIGTERM)
    terminatedaemon = 1;
};
#endif /* POSIX */

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

    GetStartupInfo(&StartupInfo);
    if (StartupInfo.lpDesktop[0] == 0)
	return issvc = 1; /* have no desktop - we are the service probably */
#endif /* WIN32SERVICE */

    return issvc = 0; /* assume not the service */
} /* is_nt_service */

HANDLE hMustTerminate = NULL;
void set_nt_exit_event(HANDLE h_svc_exit_event)
{
    hMustTerminate = h_svc_exit_event;
} /* set_nt_exit_event */

#endif /* WIN32 */

int mix_daemon(void)
{
  long t, slept;
  t = SENDPOOLTIME;
  if (MAILINTIME < t && (MAILIN != NULL && MAILIN[0] != '\0'))
    t = MAILINTIME;
#ifdef USE_SOCK
  if (POP3TIME < t)
    t = POP3TIME;
#endif /* USE_SOCK */
  if (t < 5)
    t = 5; /* Some kind of safety for broken systems */
  slept = t;

  setsignalhandler(1); /* set signal handlers and restart any interrupted system calls */
  for(;;) {
    if (terminatedaemon)
      break;
    if (slept >= t) {
      mix_regular(0);
      slept = 0;
    }

#ifdef WIN32SERVICE
    if (hMustTerminate) {
      if (WaitForSingleObject(hMustTerminate, t * 1000) == WAIT_OBJECT_0) {
	CloseHandle(hMustTerminate);
	terminatedaemon = 1;
      }
    }
#endif /* WIN32SERVICE */

    if (!terminatedaemon) {
      setsignalhandler(0); /* set signal handlers;  don't restart system calls */
#ifdef WIN32
      Sleep(t * 1000); /* how to get the real number of seconds slept? */
      slept = t;
#else /* end of WIN32 */
      slept += (t - slept) - sleep(t - slept);
#endif /* else if not WIN32 */
      setsignalhandler(1); /* set signal handlers and restart any interrupted system calls */
    }
  }
  return (0);
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
#else /* end of _MSC */
    vsnprintf(statusline, sizeof(statusline) - 1, fmt, args);
#endif /* else if not _MSC */
    va_end(args);
  }
#ifdef USE_NCURSES
  if (menu_initialized) {
    cl(LINES - 2, 10);
    printw("%s", statusline);
    refresh();
  } else
#endif /* USE_NCURSES */
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
