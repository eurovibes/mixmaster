/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Key management
   $Id: keymgt.c,v 1.7 2002/08/03 17:08:01 weaselp Exp $ */


#include "mix3.h"
#include <string.h>
#include <assert.h>

static int getv2seckey(byte keyid[], BUFFER *key);
static int getv3seckey(byte keyid[], BUFFER *key);
static int getv2pubkey(byte keyid[], BUFFER *key);
static int getv3pubkey(byte keyid[], BUFFER *key);

int db_getseckey(byte keyid[], BUFFER *key)
{
  if (getv3seckey(keyid, key) == -1 && getv2seckey(keyid, key) == -1)
    return (-1);
  else
    return (0);
}

int db_getpubkey(byte keyid[], BUFFER *key)
{
  if (getv3pubkey(keyid, key) == -1 && getv2pubkey(keyid, key) == -1)
    return (-1);
  else
    return (0);
}

static int getv3seckey(byte keyid[], BUFFER *key)
{
  return -1;			/*  XXX */
}

static int getv3pubkey(byte keyid[], BUFFER *key)
{
  return -1;			/*  XXX */
}

#ifdef USE_RSA
static int getv2seckey(byte keyid[], BUFFER *key)
{
  FILE *keyring;
  BUFFER *iv, *pass, *temp;
  char idstr[33];
  char line[LINELEN];
  int err = 0;

  pass = buf_new();
  iv = buf_new();
  temp = buf_new();
  id_encode(keyid, idstr);
  strcat(idstr, "\n");
  if ((keyring = mix_openfile(SECRING, "r")) == NULL) {
    errlog(ERRORMSG, "No secret key file!\n");
    err = -1;
    goto end;
  }
  for (;;) {
    if (fgets(line, sizeof(line), keyring) == NULL)
      break;
    if (strleft(line, begin_key)) {
      if (fgets(line, sizeof(line), keyring) == NULL)
	break;
      if (!streq(line, idstr))
	continue;
      fgets(line, sizeof(line), keyring);
      fgets(line, sizeof(line), keyring);
      buf_sets(iv, line);
      decode(iv, iv);
      for (;;) {
	if (fgets(line, sizeof(line), keyring) == NULL)
	  break;
	if (strleft(line, end_key))
	  break;
	buf_append(key, line, strlen(line) - 1);
      }
      break;
    }
  }
  fclose(keyring);

  if (key->length == 0) {
    errlog(ERRORMSG, "No such key: %s", idstr);
    err = -1;
  } else {
    err = decode(key, key);
    if (err == -1)
      errlog(ERRORMSG, "Corrupt secret key.\n");
  }
  if (err == -1)
    goto end;
  buf_sets(pass, PASSPHRASE);
  digest_md5(pass, pass);
  buf_crypt(key, pass, iv, DECRYPT);

  err = check_seckey(key, keyid);
  if (err == -1)
    errlog(ERRORMSG, "Corrupt secret key. Bad passphrase?\n");
end:
  buf_free(pass);
  buf_free(iv);
  buf_free(temp);
  return (err);
}

static int getv2pubkey(byte keyid[], BUFFER *key)
{
  FILE *keyring;
  BUFFER *b, *temp, *iv;
  char idstr[33];
  char line[LINELEN];
  int err = 0;

  b = buf_new();
  iv = buf_new();
  temp = buf_new();
  id_encode(keyid, idstr);
  strcat(idstr, "\n");
  if ((keyring = mix_openfile(PUBRING, "r")) == NULL) {
    errlog(ERRORMSG, "Can't open %s!\n", PUBRING);
    err = -1;
    goto end;
  }
  for (;;) {
    if (fgets(line, sizeof(line), keyring) == NULL)
      break;
    if (strleft(line, begin_key)) {
      if (fgets(line, sizeof(line), keyring) == NULL)
	break;
      if (!streq(line, idstr))
	continue;
      fgets(line, sizeof(line), keyring);	/* ignore length */
      for (;;) {
	if (fgets(line, sizeof(line), keyring) == NULL)
	  goto done;
	if (strleft(line, end_key))
	  goto done;
	buf_append(key, line, strlen(line) - 1);
      }
      break;
    }
  }
done:
  fclose(keyring);

  if (key->length == 0) {
    errlog(ERRORMSG, "No such public key: %s", idstr);
    err = -1;
    goto end;
  }
  err = decode(key, key);
  if (err != -1)
    err = check_pubkey(key, keyid);
  if (err == -1)
    errlog(ERRORMSG, "Corrupt public key %s", idstr);
end:
  buf_free(b);
  buf_free(iv);
  buf_free(temp);
  return (err);
}

#else
static int getv2seckey(byte keyid[], BUFFER *key)
{
  return -1;
}

static int getv2pubkey(byte keyid[], BUFFER *key)
{
  return -1;
}
#endif

int key(BUFFER *out)
{
  int err = -1;
  FILE *f;

  buf_sets(out, "Subject: Remailer key for ");
  buf_appends(out, SHORTNAME);
  buf_appends(out, "\n\n");

  keymgt(0);

  conf_premail(out);
  buf_nl(out);

  if (PGP) {
    if ((f = mix_openfile(PGPKEY, "r")) != NULL) {
      buf_appends(out, "Here is the PGP key:\n\n");
      buf_read(out, f);
      buf_nl(out);
      fclose(f);
      err = 0;
    }
  }
  if (MIX) {
    if ((f = mix_openfile(KEYFILE, "r")) != NULL) {
      buf_appends(out, "Here is the Mixmaster key:\n\n");
      buf_appends(out, "=-=-=-=-=-=-=-=-=-=-=-=\n");
      buf_read(out, f);
      buf_nl(out);
      fclose(f);
      err = 0;
    }
  }
  if (err == -1 && UNENCRYPTED) {
    buf_appends(out, "The remailer accepts unencrypted messages.\n");
    err = 0;
  }
  if (err == -1)
    errlog(ERRORMSG, "Cannot create remailer keys!");

  return (err);
}

int adminkey(BUFFER *out)
{
        int err = -1;
        FILE *f;

        buf_sets( out, "Subject: Admin key for the " );
        buf_appends( out, SHORTNAME );
        buf_appends( out, " remailer\n\n" );

        if ( (f = mix_openfile( ADMKEYFILE, "r" )) != NULL ) {
                buf_read( out, f );
                buf_nl( out );
                fclose( f );
                err = 0;
        }

        if ( err == -1 )
                errlog( ERRORMSG, "Can not read admin key file!\n" );

        return err;
}

#ifdef USE_RSA
int v2keymgt(int force)
{
  /* scan secring, write the pubkey. function will be rewritten
     for advanced key management in v3 */

  FILE *keyring, *f;
  char line[LINELEN];
  byte k1[16];
  BUFFER *b, *temp, *iv, *pass, *pk;
  int err = 0;
  int found = 0;

  b = buf_new();
  temp = buf_new();
  iv = buf_new();
  pass = buf_new();
  pk = buf_new();

  if (force == 2)
    v2createkey();
  else if ((f = mix_openfile(SECRING, "r")) == NULL)
    v2createkey();

  if (force == 0 && (f = mix_openfile(KEYFILE, "r")) != NULL) {
    fclose(f);
    goto end;
  }
  keyring = mix_openfile(SECRING, "r");
  if (keyring != NULL) {
    for (;;) {
      if (fgets(line, sizeof(line), keyring) == NULL)
	break;
      if (strleft(line, begin_key)) {
	if (fgets(line, sizeof(line), keyring) == NULL)
	  break;
	id_decode(line, k1);
	fgets(line, sizeof(line), keyring);
	if (fgets(line, sizeof(line), keyring) == NULL)
	  break;
	buf_sets(iv, line);
	decode(iv, iv);
	buf_reset(b);
	for (;;) {
	  if (fgets(line, sizeof(line), keyring) == NULL)
	    break;
	  if (strleft(line, end_key))
	    break;
	  buf_append(b, line, strlen(line) - 1);
	}
	if (decode(b, b) == -1)
	  break;
	buf_sets(temp, PASSPHRASE);
	digest_md5(temp, pass);
	buf_crypt(b, pass, iv, DECRYPT);
	if (seckeytopub(pk, b, k1) == 0)
	  found = 1;
	break;
      }
    }
    fclose(keyring);
  }
  if (found) {
    id_encode(k1, line);
    if ((f = mix_openfile(KEYFILE, "w")) != NULL) {
      fprintf(f, "%s %s %s %s %s%s\n", SHORTNAME,
	      REMAILERADDR, line, VERSION,
	      MIDDLEMAN ? "M" : "",
	      NEWS[0] == '\0' ? "C" : (strchr(NEWS, '@') ? "CNm" : "CNp"));
      fprintf(f, "\n%s\n", begin_key);
      fprintf(f, "%s\n258\n", line);
      encode(pk, 40);
      buf_write(pk, f);
      fprintf(f, "%s\n\n", end_key);
      fclose(f);
    }
  } else
    err = -1;

end:
  buf_free(b);
  buf_free(temp);
  buf_free(iv);
  buf_free(pass);
  buf_free(pk);

  return (err);
}
#endif

int keymgt(int force)
{
  /* force = 0: write key file if there is none
     force = 1: update key file
     force = 2: generate new key */
  int err = 0;

  if (REMAIL || force == 2) {
#ifdef USE_RSA
    if (MIX && (err = v2keymgt(force)) == -1)
      err = -1;
#endif
#ifdef USE_PGP
    if (PGP && (err = pgp_keymgt(force)) == -1)
      err = -1;
#endif
  }
  return (err);
}
