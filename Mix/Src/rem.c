/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Process remailer messages
   $Id: rem.c,v 1.3 2001/11/06 23:41:58 rabbi Exp $ */


#include "mix3.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef POSIX
#include <unistd.h>
#else
#include <io.h>
#endif
#ifndef _MSC
#include <dirent.h>
#endif
#include <assert.h>

int blockrequest(BUFFER *message);

#define REQUESTHELP  100
#define REQUESTSTATS 101
#define REQUESTKEY   200
#define REQUESTCONF  201
#define REQUESTOPKEY 202
#define BLOCKREQUEST 666
#define DISABLED      99

#define CPUNKMSG 1
#define MIXMSG 2

int mix_decrypt(BUFFER *message)
{
  int type = 0;
  BUFFER *field, *content;
  BUFFER *to, *subject, *replyto, *reply;
  FILE *f;
  BUFFER *block;
  int err = 0;

  mix_init(NULL);
  field = buf_new();
  content = buf_new();
  to = buf_new();
  replyto = buf_new();
  reply = buf_new();
  block = buf_new();
  subject = buf_new();
  buf_sets(subject, "Subject: Re: your mail");

  buf_rewind(message);

  f = mix_openfile(SOURCEBLOCK, "r");
  if (f != NULL) {
    buf_read(block, f);
    fclose(f);
  }
  for (;;) {
    err = buf_getheader(message, field, content);
    if (err == 1) {
      /* "::" marks for additional header lines */
      while (buf_lookahead(message, field) == 1)
	buf_getheader(message, field, content);
      if (isline(field, HDRMARK))
	continue;
      else
	goto hdrend;
    }
    if (err == -1)
      goto hdrend;

    if ((bufieq(field, "from") || bufieq(field, "sender") || bufieq(field,"received")) &&
       doblock(content, block, 1) != 0)
      goto end;

    if (bufieq(field, "to"))
      buf_cat(to, content);
    else if (bufieq(field, "from") && replyto->length == 0)
      /* reply to From address if no Reply-To header present */
      buf_set(replyto, content);
    else if (bufieq(field, "reply-to"))
      buf_set(replyto, content);
    else if (MIX && bufieq(field, "remailer-type") &&
	     bufileft(content, "mixmaster"))
      type = MIXMSG;
    else if (bufieq(field, "subject")) {
      if (bufieq(content, "help") || bufieq(content, "remailer-help"))
	type = REQUESTHELP;
      else if (bufieq(content, "remailer-stats"))
	type = REQUESTSTATS;
      else if (bufieq(content, "remailer-key"))
	type = REQUESTKEY;
      else if (bufieq(content, "remailer-adminkey"))
        type = REQUESTOPKEY;
      else if (bufieq(content, "remailer-conf"))
	type = REQUESTCONF;
      else if (bufileft(content, "destination-block"))
	type = BLOCKREQUEST;
      else {
	buf_sets(subject, "Subject: ");
	if (!bufileft(content, "re:"))
	  buf_appends(subject, "Re: ");
	buf_cat(subject, content);
      }
    } else if (bufieq(field, "test-to") || bufieq(field, "encrypted") ||
	       bufieq(field, "anon-to") ||
	       bufieq(field, "request-remailing-to") ||
	       bufieq(field, "remail-to") || bufieq(field, "anon-post-to") ||
	       bufieq(field, "post-to") || bufieq(field, "anon-send-to") ||
	       bufieq(field, "send-to") || bufieq(field, "remix-to"))
      type = CPUNKMSG;
  }
hdrend:
  if (type > 0 && REMAIL == 0)
    type = DISABLED;
  switch (type) {
  case REQUESTHELP:
    if (sendinfofile(HELPFILE, NULL, replyto, NULL) == -1)
      errlog(WARNING, "No help file available.\n");
    break;
  case REQUESTKEY:
    err = key(reply);
    if (err == 0)
      err = sendmail(reply, REMAILERNAME, replyto);
    break;
  case REQUESTOPKEY:
    err = adminkey(reply);
    if (err == 0)
      err = sendmail(reply, REMAILERNAME, replyto);
    break;
  case REQUESTSTATS:
    err = stats(reply);
    if (err == 0)
      err = sendmail(reply, REMAILERNAME, replyto);
    break;
  case REQUESTCONF:
    err = conf(reply);
    if (err == 0)
      err = sendmail(reply, REMAILERNAME, replyto);
    break;
  case CPUNKMSG:
    err = t1_decrypt(message);
    if (err != 0) {
      errlog(LOG, "Invalid type 1 message from %b\n", replyto);
      sendinfofile(USAGEFILE, USAGELOG, replyto, NULL);
      logmail(err == -2 ? MAILUSAGE : MAILERROR, message);
    }
    break;
  case MIXMSG:
    err = t2_decrypt(message);
    if (err == -1) {
      errlog(LOG, "Invalid type 2 message from %b\n", replyto);
      sendinfofile(USAGEFILE, USAGELOG, replyto, NULL);
      logmail(MAILERROR, message);
    }
    break;
  case BLOCKREQUEST:
    blockrequest(message);
    logmail(MAILBLOCK, message);
    break;
  case DISABLED:
    errlog(ERRORMSG, "Remailer is disabled.\n");
    buf_sets(reply, "Subject: remailer error\n\nThe remailer is disabled.\n");
    sendmail(reply, REMAILERNAME, replyto);
    logmail(MAILERROR, message);
    break;
  default:
    if (strifind
	(replyto->data, "mailer-daemon")) {
      errlog(LOG, "Bounce mail from %b\n", replyto);
      logmail(MAILBOUNCE, message);
    } else if (bufifind(to, REMAILERADDR) && blockrequest(message))
      logmail(MAILBLOCK, message);
    else if (!AUTOREPLY)
      logmail(MAILBOX, message);
    else if (bufifind(to, REMAILERADDR)) {
      errlog(LOG, "Non-remailer message from %b\n", replyto);
      sendinfofile(USAGEFILE, USAGELOG, replyto, NULL);
      logmail(MAILUSAGE, message);
    } else if (bufifind(to, COMPLAINTS)) {
      errlog(WARNING, "Abuse complaint from %b\n", replyto);
      sendinfofile(ABUSEFILE, NULL, replyto, subject);
      logmail(MAILABUSE, message);
    } else if (ANONADDR[0] && bufifind(to, ANONADDR)) {
      errlog(LOG, "Reply to anonymous message from %b\n", replyto);
      sendinfofile(REPLYFILE, NULL, replyto, subject);
      logmail(MAILANON, message);
    } else {
      errlog(DEBUGINFO, "Mail from %b\n", replyto);
      logmail(MAILBOX, message);
    }
    err = 1;
  }
end:
  buf_free(field);
  buf_free(content);
  buf_free(to);
  buf_free(replyto);
  buf_free(reply);
  buf_free(block);
  buf_free(subject);
  return (err);
}

int t2_decrypt(BUFFER *in)
{
  int err = 0;
  BUFFER *msg;

  msg = buf_new();
  do {
    err = mix_dearmor(in, msg);
    if (err != -1) {
      if (v3_magic(msg->data))
	err = mix3_decrypt(msg);
      else
	err = mix2_decrypt(msg);
    }
  }
  while (in->ptr + 1000 < in->length);	/* accept several packets in one message */

  buf_free(msg);
  return (err);
}

int mix_pool(BUFFER *msg, int type, long latent)
{
  char path[PATHMAX], pathtmp[PATHMAX];
  FILE *f;
  int err = -1;

  f = pool_new(latent > 0 ? "lat" : "msg", pathtmp, path);
  if (f != NULL) {
    if (latent > 0)
      fprintf(f, "%d %ld\n", type, latent + time(NULL));
    else
      fprintf(f, "%d 0\n", type);
    err = buf_write(msg, f);
    fclose(f);
  }
  if (err == 0) {
    rename(pathtmp, path);
    errlog(DEBUGINFO, "Added message to pool.\n");
  }
  return (err);
}

int pool_packetfile(char *fname, BUFFER *mid, int packetnum)
     /* create a filename */
{
#ifdef SHORTNAMES
  sprintf(fname, "%s%cp%02x%02x%02x%01x.%02x", POOLDIR, DIRSEP,
	  mid->data[0], mid->data[1], mid->data[2], mid->data[3] & 15,
	  packetnum);
#else
  sprintf(fname, "%s%cp%02x%02x%02x%02x%02x%02x%01x", POOLDIR, DIRSEP,
	  packetnum, mid->data[0], mid->data[1], mid->data[2], mid->data[3],
	  mid->data[4], mid->data[5] & 15);
#endif
  return (0);
}

void pool_packetexp(void)
{
  DIR *d;
  struct dirent *e;
  struct stat sb;

  d = opendir(POOLDIR);
  if (d != NULL)
    for (;;) {
      e = readdir(d);
      if (e == NULL)
	break;
      if (e->d_name[0] == 'p') {
	if (stat(e->d_name, &sb) == 0 &&
	    time(NULL) - sb.st_mtime > PACKETEXP) {
	  errlog(NOTICE, "Expiring partial message %s.\n",
		 e->d_name);
	  unlink(e->d_name);
	}
      }
    }
  closedir(d);
}

void logmail(char *mailbox, BUFFER *message)
{
  time_t t;
  struct tm *tc;
  char line[LINELEN];

  /* mailbox is "|program", "user@host", "stdout" or "filename" */
  buf_rewind(message);
  if (mailbox[0] == '\0')	/* default action */
    mailbox = MAILBOX;
  if (strieq(mailbox, "stdout"))
    buf_write(message, stdout);
  else if (mailbox[0] == '|') {
    FILE *p;

    errlog(DEBUGINFO, "Piping message to %s.", mailbox + 1);
    p = openpipe(mailbox + 1);
    if (p != NULL) {
      buf_write(message, p);
      closepipe(p);
    }
  } else if (strchr(mailbox, '@')) {
    BUFFER *field, *content;

    field = buf_new();
    content = buf_new();
    while (buf_getheader(message, field, content) == 0)
      if (bufieq(field, "x-loop") && bufifind(content, REMAILERADDR)) {
	errlog(WARNING, "Loop detected! Message not sent to %s.\n", mailbox);
	goto isloop;
      }
    buf_sets(content, mailbox);
    sendmail(message, NULL, content);
  isloop:
    buf_free(field);
    buf_free(content);
  } else {
    FILE *mbox;

    mbox = mix_openfile(mailbox, "a");
    if (mbox == NULL) {
      errlog(ERRORMSG, "Can't write to mail folder %s\n", mailbox);
      return;
    }
    lock(mbox);
    if (!bufileft(message, "From ")) {
      t = time(NULL);
      tc = localtime(&t);
      strftime(line, LINELEN, "From Mixmaster %a %b %d %H:%M:%S %Y\n", tc);
      fprintf(mbox, line);
    }
    buf_write(message, mbox);
    fprintf(mbox, "\n");
    unlock(mbox);
    fclose(mbox);
  }
}

int blockrequest(BUFFER *message)
{
  int request = 0, domain;
  BUFFER *from, *line, *field, *content, *addr;
  FILE *f;
  char *destblklst = (char *)malloc( strlen(DESTBLOCK)+1 );
  char *destblk;

  from = buf_new();
  line = buf_new();
  field = buf_new();
  content = buf_new();
  addr = buf_new();

  buf_rewind(message);
  while (buf_getheader(message, field, content) == 0)
    if (bufieq(field, "from"))
      buf_set(from, content);
    else if (bufieq(field, "subject"))
      buf_cat(message, content);
  while (buf_getline(message, line) != -1)
    if (bufifind(line, "destination-block")) {
      buf_clear(addr);
      request = 1;
      if (buffind(line, "@")) {
	int c = 0;

	while (!strileft(line->data + line->ptr, "block"))
	  line->ptr++;
	while (c != ' ')
	  c = tolower(buf_getc(line));
	while (c == ' ')
	  c = buf_getc(line);
	do {
	  buf_appendc(addr, c);
	  c = buf_getc(line);
	} while (c > ' ');
      } else
	buf_set(addr, from);
      if (bufieq(addr, REMAILERADDR)) {
	errlog(LOG, "Ignoring blocking request for %b from %b.\n", addr, from);
	return (2);
      }
      if (buf_ieq(addr, from))
	errlog(NOTICE, "Blocking request for %b\n", addr);
      else
	errlog(NOTICE, "Blocking request for %b from %b\n", addr, from);
      if (AUTOBLOCK) {
	domain = 0;
	if (addr->data[0] == '@') {
	  domain = 1;
	  buf_sets(line, "postmaster");
	  buf_cat(line, addr);
	  buf_move(addr, line);
	}
	buf_clear(line);
	rfc822_addr(addr, line);
	if (strchr(line->data, '@') && strchr(strchr(line->data, '@'), '.')) {
          strcpy( destblklst, DESTBLOCK );
          destblk = strtok( destblklst, " " );
          f = mix_openfile( destblk, "a" );
          free( destblklst );
	  if (f != NULL) {
	    lock(f);
	    sendinfofile(BLOCKFILE, NULL, line, NULL);
	    if (line->length) {
	      if (domain)
		fprintf(f, "%s", line->data + sizeof("postmaster") - 1);
	      else
		fprintf(f, "%s", line->data);
	    } else
	      errlog(NOTICE, "%b already blocked.\n", addr);
	    unlock(f);
	    fclose(f);
	  } else
	    errlog(ERRORMSG, "Can't write to %s.\n", DESTBLOCK);
	} else
	  errlog(WARNING, "Invalid address not added to %s: %b\n", DESTBLOCK,
		 addr);
      }
    }
  return (request);
}


int idexp(void)
{
  FILE *f;
  BUFFER *b;
  long now, then;
  char line[LINELEN];
  LOCK *i;

  if (IDEXP == 0)
    return (0);

  b = buf_new();
  f = mix_openfile(IDLOG, "r");
  if (f == NULL)
    return (-1);
  i = lockfile(IDLOG);
  now = time(NULL);
  fgets(line, sizeof(line), f);	/* replace first line */
  while (fgets(line, sizeof(line), f) != NULL) {
    sscanf(line, "%*s %ld", &then);
    if (now - then < IDEXP)
      buf_appends(b, line);
  }
  fclose(f);
  f = mix_openfile(IDLOG, "w");
  if (f != NULL) {
    fprintf(f, "expired %ld\n", now - IDEXP);
    buf_write(b, f);
    fclose(f);
  }
  unlockfile(i);
  return (0);
}