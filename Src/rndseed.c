/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Get randomness from device or user
   $Id: rndseed.c,v 1.1 2001/10/31 08:19:53 rabbi Exp $ */


#include "mix3.h"
#include <assert.h>
#include <time.h>
#include <fcntl.h>
#include <time.h>
#ifdef POSIX
#include <unistd.h>
#include <termios.h>
#else
#include <io.h>
#include <process.h>
#endif
#if defined(WIN32) || defined(MSDOS)
#include <conio.h>
#endif
#ifdef WIN32
#include <windows.h>
#endif

#define NEEDED 128

#ifndef O_NDELAY
#define O_NDELAY 0
#endif

int kbd_noecho(void)
{
#ifdef HAVE_TERMIOS
  int fd;
  struct termios attr;

  setbuf(stdin, NULL);
  fd = fileno(stdin);
  if (tcgetattr(fd, &attr) != 0)
    return (-1);
  attr.c_lflag &= ~(ECHO | ICANON);
  if (tcsetattr(fd, TCSAFLUSH, &attr) != 0)
    return (-1);
#endif
  return (0);
}

int kbd_echo(void)
{
#ifdef HAVE_TERMIOS
  int fd;
  struct termios attr;

  setvbuf(stdin, NULL, _IOLBF, BUFSIZ);
  fd = fileno(stdin);
  if (tcgetattr(fd, &attr) != 0)
    return (-1);
  attr.c_lflag |= ECHO | ICANON;
  if (tcsetattr(fd, TCSAFLUSH, &attr) != 0)
    return (-1);
#endif
  return (0);
}

void rnd_error(void)
{
  errlog(ERRORMSG,
	 "Random number generator not initialized. Aborting.\n\
Run the program interactively to seed the generator.\n");
  exit(3);
}

/* get randomness from system or user. If the application has promised that
   it will seed the RNG later, we do not ask for user input */

int rnd_seed(void)
{
  int fd = -1;
  byte b[512], c = 0;
  int bytes = 0;

#ifdef DEV_RANDOM
  fd = open(DEV_RANDOM, O_RDONLY | O_NDELAY);
#endif
  if (fd == -1) {
#if 1
    if (rnd_state == RND_WILLSEED)
      return(-1);
    if (!isatty(fileno(stdin)))
      rnd_error();
#else
#error "should initialize the prng from system ressources"
#endif
    fprintf(stderr, "Please enter some random characters.\n");
    kbd_noecho();
    while (bytes < NEEDED) {
      fprintf(stderr, "  %d     \r", NEEDED - bytes);
#ifdef HAVE_GETKEY
      if (kbhit(), *b = getkey())
#else
      if (read(fileno(stdin), b, 1) > 0)
#endif
	{
	  rnd_add(b, 1);
	  rnd_time();
	  if (*b != c)
	    bytes++;
	  c = *b;
	}
    }
    fprintf(stderr, "Thanks.\n");
#ifdef WIN32
    Sleep(1000);
#else
    sleep(1);
#endif
    kbd_echo();
  }
#ifdef DEV_RANDOM
  else {
    bytes = read(fd, b, sizeof(b));
    rnd_add(b, sizeof(b));
    close(fd);
    if (bytes < NEEDED) {
      fd = open(DEV_RANDOM, O_RDONLY);	/* re-open in blocking mode */
      if (isatty(fileno(stdin))) {
	fprintf(stderr,
		"Please move the mouse, enter random characters, etc.\n");
	kbd_noecho();
      }
      while (bytes < NEEDED) {
	if (isatty(fileno(stdin)))
	  fprintf(stderr, "  %d     \r", NEEDED - bytes);
	if (read(fd, b, 1) > 0) {
	  rnd_add(b, 1);
	  bytes++;
	}
      }
      if (isatty(fileno(stdin))) {
	fprintf(stderr, "Thanks.\n");
#ifdef WIN32
	sleep(1000);
#else
	sleep(1);
#endif
	kbd_echo();
      }
      close(fd);
    }
  }
#endif
  rnd_state = RND_SEEDED;
  return (0);
}
