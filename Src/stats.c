/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Remailer statistics
   $Id: stats.c,v 1.2 2002/01/10 23:59:16 rabbi Exp $ */


#include "mix3.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/* log that a message of type t has been received. Statistics for type 2
   messages are taken from the IDLOG instead of calling this function */
int stats_log(int t)
{
  FILE *f;

  f = mix_openfile(STATS, "a");
  if (f == NULL) {
    errlog(ERRORMSG, "Can't open %s!\n", STATS);
    return (-1);
  }
  lock(f);
  fprintf(f, "%d 1 %ld\n", t, (long) time(NULL));
  unlock(f);
  fclose(f);
  return (0);
}

/* log the current pool size after sending messages */
int stats_out(int pool)
{
  FILE *f;

  f = mix_openfile(STATS, "a");
  if (f == NULL) {
    errlog(ERRORMSG, "Can't open %s!\n", STATS);
    return (-1);
  }
  lock(f);
  fprintf(f, "p 1 %d %ld\n", pool, (long) time(NULL));
  unlock(f);
  fclose(f);
  return (0);
}

int stats(BUFFER *b)
{
  FILE *s, *f;
  char line[LINELEN];
  long now, today, then;
  time_t t;
  long updated = 0, havestats = 0;
  int msgd[3][24], msg[3][80];
  int poold[2][24], pool[2][80];
  int i, num, type;

  now = (time(NULL) / (60 * 60) + 1) * 60 * 60;
  today = (now / SECONDSPERDAY) * SECONDSPERDAY;

  for (i = 0; i < 24; i++)
    msgd[0][i] = msgd[1][i] = msgd[2][i] = poold[0][i] = poold[1][i] = 0;
  for (i = 0; i < 80; i++)
    msg[0][i] = msg[1][i] = msg[2][i] = pool[0][i] = pool[1][i] = 0;

  s = mix_openfile(STATS, "r");
  if (s != NULL) {
    lock(s);
    fscanf(s, "%ld", &updated);
    while (fgets(line, sizeof(line), s) != NULL) {
      switch (line[0]) {
      case '0':
      case '1':
      case '2':
	sscanf(line, "%d %d %ld", &type, &num, &then);
	if (now - then < 0)
	  break;		/* keep memory consistent even if the time
				   suddenly goes backwards :) */
	if (now - then < SECONDSPERDAY)
	  msgd[type][(now - then) / (60 * 60)] += num;
	else if (today - then < 80 * SECONDSPERDAY)
	  msg[type][(today - then) / SECONDSPERDAY] += num;
	if (havestats == 0 || then < havestats)
	  havestats = then;
	break;
      case 'p':
	sscanf(line, "p %d %d %ld", &num, &i, &then);
	if (now - then < 0)
	  break;
	if (now - then < SECONDSPERDAY) {
	  poold[0][(now - then) / (60 * 60)] += num;
	  poold[1][(now - then) / (60 * 60)] += i;
	} else if (today - then < 80 * SECONDSPERDAY) {
	  poold[0][(today - then) / (24 * 60 * 60)] += num;
	  poold[1][(today - then) / (24 * 60 * 60)] += i;
	}
	if (havestats == 0 || then < havestats)
	  havestats = then;
	break;
      }
    }
    unlock(s);
    fclose(s);
  }
  f = mix_openfile(IDLOG, "r");
  if (f != NULL) {
    while (fscanf(f, "%*s %ld", &then) != EOF) {
      if (then < updated || now - then < 0)
	continue;
      if (now - then < SECONDSPERDAY)
	msgd[2][(now - then) / (60 * 60)]++;
      else if (today - then < 80 * SECONDSPERDAY)
	msg[2][(today - then) / SECONDSPERDAY]++;
      if (havestats == 0 || then < havestats)
	havestats = then;
    }
    fclose(f);
  }
  if (havestats == 0) {
    if (b != NULL)
      errlog(NOTICE, "No statistics available.\n");
    return (-1);
  }
  s = mix_openfile(STATS, "w");
  if (s == NULL) {
    errlog(ERRORMSG, "Can't create %s!\n", STATS);
    return (-1);
  }
  lock(s);
  fprintf(s, "%ld\n", (long) time(NULL));	/* time of stats.log update */
  for (i = 0; i < 24; i++) {
    for (type = 0; type < 3; type++)
      if (msgd[type][i] > 0)
	fprintf(s, "%d %d %ld\n", type, msgd[type][i], now - i * 60 * 60);
    if (poold[0][i] > 0)
      fprintf(s, "p %d %d %ld\n", poold[0][i], poold[1][i], now - i * 60 * 60);
  }
  for (i = 0; i < 80; i++) {
    for (type = 0; type < 3; type++)
      if (msg[type][i] > 0)
	fprintf(s, "%d %d %ld\n", type, msg[type][i],
		today - i * 24 * 60 * 60);
    if (pool[0][i] > 0)
      fprintf(s, "p %d %d %ld\n", pool[0][i], pool[1][i],
	      today - i * 24 * 60 * 60);
  }
  unlock(s);
  fclose(s);
  if (b != NULL) {
    struct tm *gt;

    buf_sets(b, "Subject: Statistics for the ");
    buf_appends(b, SHORTNAME);
    buf_appends(b, " remailer\n\n");

    buf_appends(b, "Number of messages in the past 24 hours:\n");
    t = now;
    gt = gmtime(&t);
    for (i = 23; i >= 0; i--) {
      buf_appendf(b, "   %2dh: ", (24 + gt->tm_hour - i) % 24);
      if (MIX) {
	if (PGP || UNENCRYPTED)
	  buf_appends(b, "   Mix:");
	buf_appendf(b, "%4d", msgd[2][i]);
      }
      if (PGP)
	buf_appendf(b, "     PGP: %4d", msgd[1][i]);
      if (UNENCRYPTED)
	buf_appendf(b, "     Unencrypted:%4d", msgd[0][i]);
      if (poold[0][i] > 0)
	buf_appendf(b, "  [Pool size:%4d]", poold[1][i] / poold[0][i]);
#if 0
      else
	buf_appends(b, "  [ no remailing ]");
#endif
      buf_nl(b);
    }
    if ((today - havestats) / SECONDSPERDAY >= 1)
      buf_appends(b, "\nNumber of messages per day:\n");
    for ((i = (today - havestats) / SECONDSPERDAY) > 79 ? 79 : i;
	 i >= 1; i--) {
      t = now - i * SECONDSPERDAY;
      gt = localtime(&t);
      strftime(line, LINELEN, "%d %b: ", gt);
      buf_appends(b, line);

      if (MIX) {
	if (PGP || UNENCRYPTED)
	  buf_appends(b, "   Mix:");
	buf_appendf(b, "%4d", msg[2][i]);
      }
      if (PGP)
	buf_appendf(b, "     PGP: %4d", msg[1][i]);
      if (UNENCRYPTED)
	buf_appendf(b, "     Unencrypted:%4d", msg[0][i]);
      if (pool[0][i] > 0)
	buf_appendf(b, "  [Pool size:%4d]", pool[1][i] / pool[0][i]);
#if 0
      else
	buf_appends(b, "  [ no remailing ]");
#endif
      buf_nl(b);
    }
  }
  return (0);
}

int conf(BUFFER *out)
{
  FILE *f;
  BUFFER *b, *line;
  int flag = 0;
  b = buf_new();
  line = buf_new();

  buf_sets(out, "Subject: Capabilities of the ");
  buf_appends(out, SHORTNAME);
  buf_appends(out, " remailer\n\n");
  buf_appends(out, remailer_type);
  buf_appends(out, VERSION);
  buf_nl(out);

  if (MIX + PGP + UNENCRYPTED == 1)
    buf_appends(out, "Supported format:");
  else
    buf_appends(out, "Supported formats:\n");
  if (MIX)
    buf_appends(out, "   Mixmaster\n");
  if (PGP)
    buf_appends(out, "   Cypherpunk with PGP encryption\n");
  if (UNENCRYPTED)
    buf_appends(out, "   Cypherpunk (unencrypted)\n");

  buf_appendf(out, "Pool size: %d\n", POOLSIZE);
  if (SIZELIMIT)
    buf_appendf(out, "Maximum message size: %d kB\n", SIZELIMIT);

  f = mix_openfile(HDRFILTER, "r");
  if (f != NULL) {
    buf_read(b, f);
    fclose(f);
    while(buf_getline(b, line) != -1)
      if (line->length > 0 && line->data[0] != '#') {
	if (flag == 0) {
	  buf_appends(out, "The following header lines will be filtered:\n");
	  flag = 1;
	}
        buf_appends(out, "   ");
	if (line->length > 3 && streq(line->data + line->length - 2, "/q")) {
	  buf_append(out, line->data, line->length - 1);
	  buf_appends(out, " => delete message");
	}
	else
	  buf_cat(out, line);
	buf_nl(out);
      }
    buf_free(b);
  }
  flag = 0;
  b = readdestblk( );
  if ( b != NULL ) {
    while(buf_getline(b, line) != -1)
      if (line->length > 0 && !bufleft(line, "#") && !buffind(line, "@")) {
	/* mail addresses are not listed */
	if (flag == 0) {
	  if (NEWS[0])
	    buf_appends(out,
			"The following newsgroups/domains are blocked:\n");
	  else
	    buf_appends(out, "The following domains are blocked:\n");
	  flag = 1;
	}
	buf_appendf(out, "   %b\n", line);
      }
    if (flag == 0 && NEWS[0])
      buf_appends(out, "Note that other newsgroups may be unavailable at the remailer's news server.\n");
  }

  buf_nl(out);
  conf_premail(out);
  if ( b ) buf_free(b);
  buf_free(line);
  return (0);
}

void conf_premail(BUFFER *out)
{
  buf_appends(out, "$remailer{\"");
  buf_appends(out, SHORTNAME);
  buf_appends(out, "\"} = \"<");
  buf_appends(out, REMAILERADDR);
  buf_appendc(out, '>');
  if (PGP || UNENCRYPTED)
    buf_appends(out, " cpunk");
  if (MIX)
    buf_appends(out, " mix");
  if (MIDDLEMAN)
    buf_appends(out, " middle");
  if (PGP)
    buf_appends(out, " pgp");
  if (PGP && !UNENCRYPTED)
    buf_appends(out, " pgponly");
  if (REPGP) {
    if (REMIX == 1)
      buf_appends(out, " repgp");
    else
      buf_appends(out, " repgp2");
  }
  if (REMIX == 1)
    buf_appends(out, " remix");
  else if (REMIX)
    buf_appends(out, " remix2");
  if (PGP || UNENCRYPTED)
    buf_appends(out, " latent hash cut test");
  if (PGP) {
#ifdef USE_IDEA
    buf_appends(out, " ek");
#endif
    buf_appends(out, " ekx");
  }
#ifdef USE_IDEA
  buf_appends(out, " esub");
#endif
#if 0				/* obsolete */
#ifdef USE_NSUB
  buf_appends(out, " nsub");
#else
  buf_appends(out, " ksub");
#endif
#endif
  if (INFLATEMAX)
    buf_appendf(out, " inflt%d", INFLATEMAX);
  if (MAXRANDHOPS)
    buf_appendf(out, " rhop%d", MAXRANDHOPS);
  if (POOLSIZE >= 5)
    buf_appends(out, " reord");
  if (NEWS[0])
    buf_appends(out, " post");
  if (SIZELIMIT)
    buf_appendf(out, " klen%d", SIZELIMIT);
  buf_appends(out, "\";\n");
}
