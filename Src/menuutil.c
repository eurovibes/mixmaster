/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Menu-based user interface - utility functions
   $Id: menuutil.c,v 1.1 2001/10/31 08:19:53 rabbi Exp $ */


#include "menu.h"
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int menu_initialized = 0;

#ifdef USE_NCURSES
void cl(int y, int x)
{
  move(y, x);
  hline(' ', COLS - x);
}
#endif

void menu_init(void)
{
#ifdef USE_NCURSES
  initscr();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  menu_initialized = 1;
#endif
}

void menu_exit(void)
{
  user_delpass();
#ifdef USE_NCURSES
  endwin();
#endif
}

#ifdef USE_NCURSES
void askfilename(char *path)
{
  char line[PATHMAX];

  printw("\rFile name: ");
  echo();
  wgetnstr(stdscr, path, PATHMAX);
  noecho();
  printw("\r");
  if (path[0] == '~') {
    char *h;

    if ((h = getenv("HOME")) != NULL) {
      strncpy(line, h, PATHMAX);
      strcatn(line, "/", PATHMAX);
      strcatn(line, path + 1, PATHMAX);
      strncpy(path, line, PATHMAX);
    }
  }
}

void savemsg(BUFFER *message)
{
  char savename[PATHMAX];
  FILE *f;

  askfilename(savename);
  f = fopen(savename, "a");
  if (f != NULL) {
    buf_write(message, f);
    fclose(f);
  }
}

#endif

int menu_getuserpass(BUFFER *b, int mode)
{
#ifdef USE_NCURSES
  char p[LINELEN];

  if (menu_initialized) {
    cl(LINES - 1, 10);
    if (mode == 0)
      printw("enter passphrase: ");
    else
      printw("re-enter passphrase: ");
    wgetnstr(stdscr, p, LINELEN);
    cl(LINES - 1, 10);
    refresh();
    if (mode == 0)
      buf_appends(b, p);
    else
      return (bufeq(b, p));
    return (0);
  }
#endif
  return (-1);
}
