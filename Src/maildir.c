/* Maildir support for Mixmaster 3 - see
   http://www.qmail.org/man/man5/maildir.html and
   http://cr.yp.to/proto/maildir.html
   
   hacked by drt@un.bewaff.net - http://c0re.jp/

   To test it try:
   $ gcc maildir.c -DUNITTEST -o test_maildir
   $ ./test_maildir
   this should print a single line saying "OK"
*/

#include "mix3.h"

#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdarg.h>

static unsigned long namecounter = 0;

int checkDirectory(char *dir, char *append, int create) {
  char tmp[PATHMAX];
  struct stat buf;
  int err;
  
  tmp[0] = '\0';
  strncat(tmp, dir, PATHMAX);
  tmp[PATHMAX-1] = '\0';
  if (append) {
    strncat(tmp, append, PATHMAX);
    tmp[PATHMAX-1] = '\0';
  }

  err = stat(tmp, &buf);
  if (err == -1) {
    if (create) {
#ifndef POSIX
      err = mkdir(tmp);
#else
      err = mkdir(tmp, S_IRWXU);
#endif
      if (err == 0)
	errlog(NOTICE, "Creating directory %s.\n", tmp);
    } else
      err = 1;
  } else if (!S_ISDIR(buf.st_mode))
    err = -1;

  return err;
}

/* Write "message" to "maildir", retunr 0 on success, -1 on failure */
int maildirWrite(char *maildir, BUFFER *message, int create) {
  int fd;
  int currDir;
  int count;
  int returnValue;
  char hostname[64];
  struct stat statbuf;
  char basename[100]; /* actual length should be smaller than 98 bytes */
  char tmpname[110];  /* actual length should be smaller than 102 bytes */
  char newname[110];  /* actual length should be smaller than 102 bytes */

  /* Declare a handler for SIGALRM so we can time out. */ 
  /* set_handler(SIGALRM, alarm_handler);  */
  /* alarm(86400); */
 
  hostname[0] = '\0';
  gethostname(hostname, 63);
  hostname[63] = '\0';

  if ((checkDirectory(maildir, NULL, create) != 0) ||
      (checkDirectory(maildir, "tmp", create) != 0) ||
      (checkDirectory(maildir, "cur", create) != 0) ||
      (checkDirectory(maildir, "new", create) != 0)) {
    returnValue = -1;
    goto realend;
  }

  /* Step 1: chdir to maildir (and save actual dir by opening it) */
  currDir = open(".", O_RDONLY);
  if(chdir(maildir) != 0) {
    returnValue = -1;
    goto functionExit;
  }
  
  /* Step 2:  Stat the temporary file.  Wait for ENOENT as a response. */ 
  for (count = 0;; count++) {
    tmpname[0] = '\0';
    newname[0] = '\0';
    sprintf(basename, "%lu.%lu-%u.%s", time(NULL), namecounter, getpid(), hostname);
    strcat(tmpname, "tmp/");
    strncat(tmpname, basename, 100);
    strcat(newname, "new/");
    strncat(newname, basename, 100);
    namecounter++;
	
    if (stat(tmpname, &statbuf) == 0)
      errno = EEXIST;
    else if (errno == ENOENT) {
      /* Step 4: create the file (at least try) */
      fd = open(tmpname, O_WRONLY|O_CREAT|O_EXCL, S_IWUSR|S_IRUSR); 
      if (fd >= 0) 
	break; /* we managed to open the file */
    }

    if (count > 5) {
      /* Too many retries - give up */	  
      errlog(ERRORMSG, "Can't create message in %s\n", maildir);
      returnValue = -1;
      goto functionExit;
    }

    /* Step 3: sleep and retry */
    sleep(2);
  }
 
  /* Step 5:  write file */ 
  if(write(fd, message->data, message->length) != message->length) {
    returnValue = -1;
    goto functionExit;
  }

  /* on NFS this could fail */
#ifndef WIN32
  if((fsync(fd) != 0) || (close(fd) != 0)) {
#else
  if((_commit(fd) != 0) || (close(fd) != 0)) {
#endif
    returnValue = -1;
    goto functionExit;
  } 

  /* Step 6: move message to 'cur' */
#ifdef POSIX
  if(link(tmpname, newname) != 0) {
    if (errno == EXDEV) {
      /* We probably are on coda or some other filesystem that does not allow
       * hardlinks. rename() the file instead of link() and unlink()
       * I know, It's evil (PP).
       */
      if (rename(tmpname, newname) != 0) {
	returnValue = -1;
	goto functionExit;
      };
    }
    returnValue = -1;
    goto functionExit;
  } else {
    if(unlink(tmpname) != 0) {
      /* unlinking failed */
      returnValue = -1;
      goto functionExit;
    }
  }
#else /* POSIX */
  /* On non POSIX systems we simply use rename(). Let's hobe DJB
   * never finds out
   */
  if (rename(tmpname, newname) != 0) {
    returnValue = -1;
    goto functionExit;
  };
#endif /* POSIX */

  returnValue = 0;

functionExit:
  /* return to original directory */
  if ((fchdir(currDir) != 0) || (close(currDir) != 0))
    returnValue = -1;
  
realend:
  
  return returnValue;
}

#ifdef UNITTEST

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <dirent.h>
#include <assert.h>

/* mock-up of errlog for unittest */
void errlog(int type, char *fmt,...)
{
  va_list ap;
  
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}

/* main for unittest */
int main()
{
  int i, count = 23;
  int fd;
  DIR *d;
  struct dirent *de;
  BUFFER message;
  char text[] = "From: nobody@un.bewaff.net\nTo: hackers@c0re.jp\nSubject: testing\n\nthis is just a test\n";
  char buf[1024];

  /* create buffer with test data */
  message.data = text;
  message.length = strlen(text);

  /* write <count> messages to maildir */
  for(i = 0; i < count; i++)
    assert(maildirWrite("Maildir.test_maildir", message, 1) == 0);

  /* read them back */
  assert((d = opendir("Maildir.test_maildir/new")) != NULL);
  for (i = 0; i < count + 2; i++) 
    {
      de = readdir(d);
      if(de->d_name[0] != '.')
	{
	  buf[0] = '\0';
	  strcat(buf, "Maildir.test_maildir/new/");
	  strcat(buf, de->d_name);
	  fd = open(buf, O_RDONLY);
	  assert(unlink(buf) == 0);
	  assert(read(fd, buf, strlen(text)) == strlen(text));
	  buf[strlen(text)] = '\0';
	  /* check if they match the original message */
	  assert(strcmp(text, buf) == 0);
	  close(fd);
	}
    }
  
  /* no files left in directory? */
  assert(readdir(d) == NULL);

  /* delete maildir */
  assert(rmdir("Maildir.test_maildir/tmp") == 0);
  assert(rmdir("Maildir.test_maildir/new") == 0);
  assert(rmdir("Maildir.test_maildir/cur") == 0);
  assert(rmdir("Maildir.test_maildir") == 0);

  /* check if writing to a non existant maildir yilds an error */
  assert(maildirWrite("Maildir.test_maildir", &message, 0) == -1);
  
  puts("OK");
}
#endif /* UNITTEST */
