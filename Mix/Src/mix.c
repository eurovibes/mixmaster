/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Mixmaster initialization, configuration
   $Id: mix.c,v 1.4 2001/12/12 18:50:09 rabbi Exp $ */


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

/** config ***************************************************************/

char MIXDIR[PATHMAX];
char POOLDIR[PATHMAX];

/* programs */
#ifdef WIN32
char SENDMAIL[LINELEN] = "outfile";
#else
char SENDMAIL[LINELEN] = "/usr/lib/sendmail -t";
#endif
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

#ifdef USE_SOCK
char HELONAME[LINELEN];
char ENVFROM[LINELEN];
int POP3DEL = 0;
int POP3SIZELIMIT = 0;
long POP3TIME = 60 * 60;

#endif

char SHORTNAME[LINELEN];

/* remailer configuration */
int REMAIL = 1;
int MIX = 1;
int PGP = 1;
int UNENCRYPTED = 0;
int REMIX = 1;

int POOLSIZE = 0;
int RATE = 100;
int MIDDLEMAN = 0;
int AUTOBLOCK = 1;
char FORWARDTO[LINELEN] = "*";
int SIZELIMIT = 0;		/* maximal size of remailed messages */
int INFLATEMAX = 50;		/* maximal size of Inflate: padding */
int MAXRANDHOPS = 20;
int BINFILTER = 0;		/* filter binary attachments? */
long PACKETEXP = 7 * SECONDSPERDAY;	/* Expiration time for old packets */
long IDEXP = 7 * SECONDSPERDAY;	/* 0 = no ID log !! */
long SENDPOOLTIME = 60 * 60;	/* frequency for sending pool messages */

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
char MAILBOX[PATHMAX] = "mbox";
char MAILABUSE[PATHMAX];
char MAILBLOCK[PATHMAX];
#ifdef WIN32
char MAILUSAGE[PATHMAX] = "nul:";
char MAILANON[PATHMAX] = "nul:";
char MAILERROR[PATHMAX] = "nul:";
#else
char MAILUSAGE[PATHMAX] = "/dev/null";
char MAILANON[PATHMAX] = "/dev/null";
char MAILERROR[PATHMAX] = "/dev/null";
#endif
char MAILBOUNCE[PATHMAX];

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

#if defined(HAVE_GETDOMAINNAME) || defined(HAVE_GETHOSTNAME)
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
	  read_conf(MAILtoNEWS) || read_conf(ERRLOG) ||
	  read_conf(ORGANIZATION) || read_conf(MID) ||
	  read_conf(TYPE1) || read_conf_i(POOLSIZE) ||
	  read_conf_i(RATE) || read_conf_i(MIDDLEMAN) ||
	  read_conf_i(AUTOBLOCK) || read_conf(FORWARDTO) ||
	  read_conf_i(SIZELIMIT) || read_conf_i(INFLATEMAX) ||
	  read_conf_i(MAXRANDHOPS) || read_conf_i(BINFILTER) ||
	  read_conf_t(PACKETEXP) || read_conf_t(IDEXP) ||
	  read_conf_t(SENDPOOLTIME) || read_conf_i(NUMCOPIES) ||
	  read_conf(CHAIN) || read_conf_i(VERBOSE) ||
	  read_conf_i(DISTANCE) || read_conf_i(MINREL) ||
	  read_conf_i(RELFINAL) || read_conf_t(MAXLAT) ||
	  read_conf(PGPPUBRING) || read_conf(PGPSECRING) ||
#ifdef USE_SOCK
	  read_conf_i(POP3DEL) || read_conf_i(POP3SIZELIMIT) ||
	  read_conf_t(POP3TIME) ||
#endif
	  read_conf(MAILBOX) || read_conf(MAILABUSE) ||
	  read_conf(MAILBLOCK) || read_conf(MAILUSAGE) ||
	  read_conf(MAILANON) || read_conf(MAILERROR) ||
	  read_conf(MAILBOUNCE));
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

#ifdef POSIX
  pw = getpwuid(getuid());
#endif

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
#endif

  if (err == -1 && (d = getenv("MIXPATH")) != NULL)
    err = mixdir(d, 1);

#ifdef SPOOL
  if (err == -1 && strlen(SPOOL) > 0)
    err = mixdir(SPOOL, 0);
#endif

#ifdef POSIX
  if (err == -1 && pw != NULL) {
    strcatn(line, pw->pw_dir, PATHMAX);
    if (line[strlen(line) - 1] != DIRSEP)
      strcatn(line, DIRSEPSTR, PATHMAX);
    strcatn(line, "Mix", PATHMAX);
    err = mixdir(line, 1);
  }
#endif

  if (err == -1) {
    getcwd(MIXDIR, PATHMAX);
    mixdir(MIXDIR, 0);
  }

  strncpy(POOLDIR, MIXDIR, PATHMAX - 32);
  strcatn(POOLDIR, POOL, PATHMAX);
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
    if (now - tpool > SENDPOOLTIME)
      force |= FORCE_POOL;
#ifdef USE_SOCK
    if (now - tpop3 > POP3TIME)
      force |= FORCE_POP3;
#endif
    if (now - tdaily > SECONDSPERDAY)
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
  long t;
  t = SENDPOOLTIME;
#ifdef USE_SOCK
  if (POP3TIME < t)
    t = POP3TIME;
#endif

  for(;;) {
    mix_regular(0);
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
    sleep(t);
#endif
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
