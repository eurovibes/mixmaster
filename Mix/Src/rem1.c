/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Process Cypherpunk remailer messages
   $Id: rem1.c,v 1.2 2001/12/11 06:05:34 rabbi Exp $ */


#include "mix3.h"
#include <ctype.h>
#include <time.h>
#include <string.h>

static int t1msg(BUFFER *in, int hdr);

int isline(BUFFER *line, char *text)
{
  int i;

  if (!bufileft(line, text))
    return (0);

    for (i = strlen(text); i < line->length; i++)
      if (!isspace(line->data[i]))
	return(0);
    return(1);
}

int t1_decrypt(BUFFER *in)
{
  int ret;

  buf_rewind(in);
  if (TYPE1[0] == '\0')
    ret = t1msg(in, 1);
  else {
    FILE *f;

    f = openpipe(TYPE1);
    if (f == NULL)
      return -1;
    buf_write(in, f);
    ret = closepipe(f);
  }
  if (ret == 0)
    stats_log(1);
  return (ret);
}

#ifdef USE_IDEA
void t1_esub(BUFFER *esub, BUFFER *subject)
{
  BUFFER *iv, *out;
  char hex[33];

  iv = buf_new();
  out = buf_new();

  buf_appendrnd(iv, 8);
  id_encode(iv->data, hex);
  buf_append(out, hex, 16);

  digest_md5(esub, esub);
  digest_md5(subject, subject);
  buf_ideacrypt(subject, esub, iv, ENCRYPT);
  id_encode(subject->data, hex);
  buf_appends(out, hex);
  buf_move(subject, out);
  buf_free(iv);
  buf_free(out);
}
#endif

#define N(X) (isdigit(X) ? (X)-'0' : 0)

static int readnum(BUFFER *b, int f)
{
  int num = 0;

  if (b->length > 0)
    sscanf(b->data, "%d", &num);
  num *= f;
  if (strchr(b->data, 'r'))
    num = rnd_number(num) + 1;
  return (num);
}

static int t1msg(BUFFER *in, int hdr)
     /* hdr = 1: mail header, hdr = 2: pasted header, hdr = 0: ignore */
{
  BUFFER *field, *content, *line;
  BUFFER *cutmarks, *to, *newsgroups, *ek, *ekdes, *ekcast, *esub, *subject;
  BUFFER *temp, *header, *out;
  BUFFER *test, *testto, *remixto;
  int err = 0;
  int encrypted = 0;
  int type = -1;
  int latent = 0;
  int remix = 0;
  int inflate = 0;

  field = buf_new();
  content = buf_new();
  line = buf_new();
  to = buf_new();
  remixto = buf_new();
  cutmarks = buf_new();
  newsgroups = buf_new();
  ek = buf_new();
  ekdes = buf_new();
  ekcast = buf_new();
  esub = buf_new();
  subject = buf_new();
  temp = buf_new();
  header = buf_new();
  out = buf_new();
  test = buf_new();
  testto = buf_new();

  if (REMIX == 1)
    remix = 2;
  if (!UNENCRYPTED)
    encrypted = -1;

header:
  while (buf_getheader(in, field, content) == 0) {
    if (header->length == 0 && bufieq(content, ":"))	/* HDRMARK */
      hdr = 2;

    if (bufieq(field, "test-to"))
      buf_set(testto, content);
    else if (PGP && bufieq(field, "encrypted"))
      encrypted = 1;
    else if (bufieq(field, "remix-to")) {
      remix = 1;
      buf_set(remixto, content);
      if (type == -1)
	type = MSG_MAIL;
    } else if (bufieq(field, "anon-to") ||
	       bufieq(field, "request-remailing-to") ||
	       bufieq(field, "remail-to") ||
	       bufieq(field, "anon-send-to")) {
      if (bufieq(field, "remail-to"))
	remix = 0;
      if (to->length > 0)
	buf_appendc(to, ',');
      buf_cat(to, content);
      if (type == -1)
	type = MSG_MAIL;
    } else if (bufieq(field, "anon-post-to") || bufieq(field, "post-to")) {
      if (newsgroups->length > 0)
	buf_appendc(newsgroups, ',');
      buf_cat(newsgroups, content);
      type = MSG_POST;
    } else if (bufieq(field, "cutmarks"))
      buf_set(cutmarks, content);
    else if (bufieq(field, "latent-time")) {
      byte *q;
      int l;

      q = content->data;
      l = strlen(q);
      latent = 0;
      if (q[0] == '+')
	q++;
      if (l >= 5 && q[2] == ':')
	latent = 600 * N(q[0]) + 60 * N(q[1]) + 10 * N(q[3]) + N(q[4]);
      else if (l >= 4 && q[1] == ':')
	latent = 60 * N(q[0]) + 10 * N(q[2]) + N(q[3]);
      else if (l >= 3 && q[0] == ':')
	latent = 10 * N(q[1]) + N(q[2]);
      if (!bufleft(content, "+")) {
	time_t now;

	time(&now);
	latent -= localtime(&now)->tm_hour * 60;
	if (latent < 0)
	  latent += 24 * 60;
      }
      if (q[l - 1] == 'r')
	latent = rnd_number(latent);
    } else if (bufieq(field, "null"))
      type = MSG_NULL;
    else if (bufieq(field, "encrypt-key") || bufieq(field, "encrypt-idea"))
      buf_set(ek, content);
    else if (bufieq(field, "encrypt-des") || bufieq(field, "encrypt-3des"))
      buf_set(ekdes, content);
    else if (bufieq(field, "encrypt-cast") || bufieq(field, "encrypt-cast5"))
      buf_set(ekcast, content);
    else if (bufieq(field, "encrypt-subject"))
      buf_set(esub, content);
    else if (bufieq(field, "inflate")) {
      inflate = readnum(content, 1024);
      if (inflate > INFLATEMAX * 1024)
	inflate = INFLATEMAX * 1024;
    } else if (bufieq(field, "rand-hop")) {
      int randhops, i;
      randhops = readnum(content, 1);
      if (randhops > MAXRANDHOPS)
	randhops = MAXRANDHOPS;
      buf_clear(temp);
      if (remixto->length)
	 buf_move(temp, remixto);
      for (i = 0; i < randhops; i++) {
	if (remixto->length > 0)
	  buf_appendc(remixto, ',');
	buf_appendc(remixto, '*');
      }
      if (temp->length) {
	buf_appendc(remixto, ',');
	buf_cat(remixto, temp);
      }
    }
#if USE_NSUB
    else if (bufieq(field, "subject"))
      buf_set(subject, content);
#endif
  }

  if (cutmarks->length > 0) {
    BUFFER *cut;

    cut = buf_new();
    buf_clear(temp);

    while ((err = buf_getline(in, line)) != -1 && !buf_eq(line, cutmarks)) {
      buf_cat(temp, line);
      buf_nl(temp);
    }
    while (err != -1) {
      err = buf_getline(in, line);
      if (err == -1 || buf_eq(line, cutmarks)) {
	t1msg(cut, 0);
	buf_clear(cut);
      } else {
	buf_cat(cut, line);
	buf_nl(cut);
      }
    }
    buf_move(in, temp);
    buf_clear(cutmarks);
  }
  if (encrypted == 1) {
#ifdef USE_PGP
    err = pgp_dearmor(in, temp);
    if (err == 0) {
      BUFFER *pass;

      pass = buf_new();
      buf_sets(pass, PASSPHRASE);
      err = pgp_decrypt(temp, pass, NULL, NULL, NULL);
      buf_free(pass);
    }
    if (err != -1 && temp->length == 0) {
      errlog(ERRORMSG, "Empty PGP message.\n");
      err = -1;
      goto end;
    }
    if (err != -1) {
      buf_rest(temp, in);	/* dangerous, but required for reply blocks */
      buf_move(in, temp);
      encrypted = 0;
      hdr = 0;
      goto header;
    }
#endif
    if (testto->length == 0)
      errlog(ERRORMSG, "Can't decrypt PGP message.\n");
    buf_appends(test, "Can't decrypt PGP message.\n");
  }
  while ((err = buf_lookahead(in, line)) == 1)
    buf_getline(in, line);
#if 0
  if (err == -1)
    goto end;
#endif

  if (isline(line, HDRMARK) && (hdr == 0 || hdr == 1)) {
    buf_getline(in, NULL);
    hdr = 2;
    goto header;
  } else if (isline(line, HASHMARK)) {
    buf_getline(in, NULL);
    for (;;) {
      if (buf_lookahead(in, line) == 0 && bufileft(line, "subject:")) {
	buf_getheader(in, field, content);
	buf_set(subject, content);
      }
      if (buf_getline(in, line) != 0)
	break;
      buf_cat(header, line);
      buf_nl(header);
    }
  }
  if (encrypted == -1) {
    if (testto->length == 0)
      errlog(LOG, "Unencrypted message detected.\n");
    buf_appends(test, "Unencrypted message detected.\n");
    err = -2;
    goto end;
  }
  if (type == MSG_POST && subject->length == 0)
    buf_sets(subject, "(no subject)");

  if (to->length > 0)
    buf_appendf(out, "To: %b\n", to);
  else if (remixto->length > 0)
    buf_appendf(out, "To: %b\n", remixto);
  if (newsgroups->length > 0)
    buf_appendf(out, "Newsgroups: %b\n", newsgroups);
  if (subject->length > 0) {
#ifdef USE_IDEA
    if (esub->length > 0)
      t1_esub(esub, subject);
#endif
    buf_appendf(out, "Subject: %b\n", subject);
  }
  buf_cat(out, header);
  buf_nl(out);

#if 0
  inflate -= in->length;
#endif
  if (inflate > 0) {
    buf_setrnd(temp, inflate * 3 / 4);
    encode(temp, 64);
    buf_appends(in, "\n-----BEGIN GARBAGE-----\n");
    buf_cat(in, temp);
    buf_appends(in, "-----END GARBAGE-----\n");
  }

  if (!(ek->length || ekdes->length || ekcast->length))
    buf_rest(out, in);
  else {
    err = 0;
    buf_clear(temp);
    while (buf_getline(in, line) != -1) {
      if (isline(line, EKMARK)) {
	buf_cat(out, temp);
	buf_clear(temp);
	buf_rest(temp, in);
	break;
      }
      else {
	buf_cat(temp, line);
	buf_nl(temp);
      }
    }
#ifdef USE_PGP
    if (ekcast->length) {
      err = pgp_encrypt(PGP_CONVCAST | PGP_TEXT, temp, ekcast, NULL, NULL,
			NULL, NULL);
      buf_clear(ekcast);
    }
    if (ekdes->length) {
      err = pgp_encrypt(PGP_CONV3DES | PGP_TEXT, temp, ekdes, NULL, NULL,
			NULL, NULL);
      buf_clear(ekdes);
    }
    if (ek->length) {
      err = pgp_encrypt(PGP_CONVENTIONAL | PGP_TEXT, temp, ek, NULL, NULL,
			NULL, NULL);
      buf_clear(ek);
    }
    buf_appends(out, EKMARK);
    buf_nl(out);
    buf_cat(out, temp);
#else
    err = -1;
#endif
  }

  if (type == -1) {
    buf_appends(test, "No destination.\n");
    err = -1;
  }

end:
  if (testto->length) {
    BUFFER *report;
    int i;

    report = buf_new();
    buf_sets(report,
	     "Subject: remailer test report\n\nThis is an automated response to the test message you sent to ");
    buf_appends(report, SHORTNAME);
    buf_appends(report, ".\nYour test message results follow:\n\n");
    buf_appends(report, remailer_type);
    buf_appends(report, VERSION);
    buf_appends(report, "\n\n");
    if (err == 0) {
      err = filtermsg(out);
      if (err == -1)
	buf_appends(report, "This remailer cannot deliver the message.\n\n");
      else {
	buf_appends(report, "Valid ");
	buf_appends(report, type == MSG_POST ? "Usenet" : "mail");
	buf_appends(report, " message.\n");
	if (remixto->length) {
	  if (remix && MIX)
	    buf_appends(report, "Delivery via Mixmaster: ");
	  else if (remix)
	    buf_appends(report, "Error! Can't remix: ");
	  else
	    buf_appends(report, "Delivery via Cypherpunk remailer: ");
	  buf_cat(report, remixto);
	  buf_nl(report);
	}
	else if (type == MSG_POST && strchr(NEWS, '@') && !strchr(NEWS, ' ')) {
	  buf_appendf(report, "News gateway: %s\n", NEWS);
	}
	buf_appends(report,
		    "\n=========================================================================\nThe first 20 lines of the message follow:\n");
	if (err != 1)
	  buf_appendf(report, "From: %s\n", ANONNAME);
	if (type == MSG_POST && ORGANIZATION[0] != '\0')
	  buf_appendf(report, "Organization: %s\n", ORGANIZATION);
      }
      for (i = 0; i < 20 && buf_getline(out, test) != -1; i++)
	buf_cat(report, test), buf_nl(report);
    } else {
      buf_appends(report, "The remailer message is invalid.\n\n");
      if (test->length) {
	buf_appends(report, "The following error occurred: ");
	buf_cat(report, test);
	buf_nl(report);
      }
    }
    buf_appends(report,
		"=========================================================================\nThe first 20 lines of your message to the remailer follow:\n");
    buf_rewind(in);
    for (i = 0; i < 20 && buf_getline(in, test) != -1; i++)
      buf_cat(report, test), buf_nl(report);

    sendmail(report, REMAILERNAME, testto);
    err = 0;
    buf_free(report);
  } else if (err == 0 && type != MSG_NULL) {
    err = 1;
    if (bufieq(to, REMAILERADDR)) /* don't remix to ourselves */
      remix = 0;
    if (remix) {
      if (remixto->length) {
	err = mix_encrypt(type, out, remixto->data, 1, line);
	if (err != 0)
	  errlog(NOTICE, "Can't remix-to %b -- %b\n", remixto->data, line);
      } else /* transparent remixing */ {
	err = mix_encrypt(type, out, to->data, 1, line);
	if (err != 0) {
	  errlog(DEBUGINFO, "Can't transparently remix to %b\n", to);
	  err = mix_pool(out, type, latent * 60);
	}
      }
    } else {
      err = mix_pool(out, type, latent * 60);
    }
  }

  buf_free(field);
  buf_free(content);
  buf_free(line);
  buf_free(to);
  buf_free(remixto);
  buf_free(newsgroups);
  buf_free(subject);
  buf_free(ek);
  buf_free(ekcast);
  buf_free(ekdes);
  buf_free(esub);
  buf_free(cutmarks);
  buf_free(temp);
  buf_free(out);
  buf_free(header);
  buf_free(test);
  buf_free(testto);
  return (err);
}
