/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Menu-based user interface
   $Id: menu.h,v 1.2 2002/07/21 03:53:11 weaselp Exp $ */


#ifndef _MENU_H
#define _MENU_H
#include "mix3.h"
#ifdef USE_NCURSES
#ifdef HAVE_NCURSES_H
#include <ncurses.h>
#else
#include <curses.h>
#endif
#endif

#define NONANON "non-anonymous"
#define ANON "Anonymous"

void send_message(int type, char *nym, BUFFER *txt);
void read_folder(char command, char *foldername, char *nym);
void menu_init(void);
void menu_exit(void);

#ifdef USE_NCURSES
void read_message(BUFFER *message, char *nym);
void menu_nym(char *);
void menu_chain(char *chain, int type, int post);
void cl(int y, int x);
void askfilename(char *fn);
void savemsg(BUFFER *message);
int menu_replychain(int *d, int *l, char *mdest, char *pdest, char *psub,
		    char *r);

#endif

#define maxnym 30

#endif
