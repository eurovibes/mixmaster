/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   OpenPGP key database
   $Id: pgpdb.c,v 1.6 2002/08/16 19:03:37 rabbi Exp $ */


#include "mix3.h"
#ifdef USE_PGP
#include "pgp.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int pgp_readkeyring(BUFFER *keys, char *filename)
{
  FILE *keyfile;
  BUFFER *armored, *line, *tmp;
  int err = -1;

  if ((keyfile = mix_openfile(filename, "rb")) == NULL)
    return (err);

  armored = buf_new();
  buf_read(armored, keyfile);
  fclose(keyfile);
  if (pgp_ispacket(armored)) {
    err = 0;
    buf_move(keys, armored);
  } else {
    line = buf_new();
    tmp = buf_new();

    while (1) {
      do
	if (buf_getline(armored, line) == -1) {
	  goto end_greedy_dearmor;
	}
      while (!bufleft(line, begin_pgp)) ;
      buf_clear(tmp);
      buf_appends(tmp, begin_pgp);
      buf_appends(tmp, "\n");
      buf_cat(tmp, armored);

      if (pgp_dearmor(tmp, tmp) == 0) {
	err = ARMORED;
	buf_cat(keys, tmp);
      }
    }
end_greedy_dearmor:
    buf_free(line);
    buf_free(tmp);
  }
  buf_free(armored);
  return (err);
}

KEYRING *pgpdb_open(char *keyring, BUFFER *encryptkey, int writer)
{
  KEYRING *keydb;

  keydb = pgpdb_new(keyring, -1, encryptkey);
  if (writer)
    keydb->lock = lockfile(keyring);
  keydb->filetype = pgp_readkeyring(keydb->db, keyring);
#if 0
  if (keydb->filetype == -1) {
    pgpdb_close(keydb);
    return (NULL);
  }
#endif
  if (encryptkey && encryptkey->length && pgp_isconventional(keydb->db) &&
      pgp_decrypt(keydb->db, encryptkey, NULL, NULL, NULL) < 0) {
    user_delpass();
    return (NULL);
  }
  return (keydb);
}

KEYRING *pgpdb_new(char *keyring, int filetype, BUFFER *encryptkey)
{
  KEYRING *keydb;

  keydb = malloc(sizeof(KEYRING));

  if (keydb == NULL)
    return NULL;
  keydb->db = buf_new();
  keydb->modified = 0;
  keydb->lock = NULL;
  strncpy(keydb->filename, keyring, sizeof(keydb->filename));
  keydb->filetype = filetype;
  if (encryptkey == NULL)
    keydb->encryptkey = NULL;
  else {
    keydb->encryptkey = buf_new();
    buf_set(keydb->encryptkey, encryptkey);
  }
  return (keydb);
}

int pgpdb_close(KEYRING *keydb)
{
  int err = 0;

  if (keydb->modified) {
    FILE *f;

    if (keydb->encryptkey && keydb->encryptkey->length)
      pgp_encrypt(PGP_NCONVENTIONAL | PGP_NOARMOR, keydb->db,
		  keydb->encryptkey, NULL, NULL, NULL, NULL);
    if (keydb->filetype == ARMORED)
      pgp_armor(keydb->db, 2);
    if (keydb->filetype == -1 || (f = mix_openfile(keydb->filename,
						   keydb->filetype ==
						   ARMORED ? "w" : "wb"))
	== NULL)
      err = -1;
    else {
      err = buf_write(keydb->db, f);
      fclose(f);
    }
  }
  if (keydb->lock)
    unlockfile(keydb->lock);
  if (keydb->encryptkey)
    buf_free(keydb->encryptkey);
  buf_free(keydb->db);
  free(keydb);
  return (err);
}

int pgpdb_getnext(KEYRING *keydb, BUFFER *key, BUFFER *keyid, BUFFER *userid)
     /* store next key from keydb with specified keyid/userid in key. */
{
  int found = 0;
  int type;
  long ptr;
  int tempbuf = 0;
  BUFFER *p, *i, *thisid;

  p = buf_new();
  i = buf_new();
  thisid = buf_new();
  
  if (key == NULL) {
    tempbuf = 1;
    key = buf_new();
  }
  assert(key != keyid);
  while (!found) {
    buf_clear(key);
    type = pgp_getpacket(keydb->db, key);
    if (type == -1)
      break;
    if (type != PGP_PUBKEY && type != PGP_SECKEY)
      continue;
    if ((keyid == NULL || keyid->length == 0) &&
	(userid == NULL || userid->length == 0))
      found = 1;

    if (keyid && keyid->length > 0) {
      pgp_keyid(key, thisid);
      if (buf_eq(keyid, thisid))
	found = 1;
    }

    pgp_packet(key, type);

    while ((ptr = keydb->db->ptr, type = pgp_getpacket(keydb->db, p)) > 0) {
      switch (type) {
      case PGP_SECKEY:
      case PGP_PUBKEY:
	keydb->db->ptr = ptr;
	goto nextkey;
      case PGP_PUBSUBKEY:
      case PGP_SECSUBKEY:
	if (keyid && keyid->length > 0) {
	  pgp_keyid(p, thisid);
	  if (buf_eq(keyid, thisid))
	    found = 1;
	}
	break;
      case PGP_USERID:
#ifdef DEBUG
	printf("%s\n", p->data);
#endif
	if (userid && userid->length > 0 && bufifind(p, userid->data))
	  found = 1;
	break;
      }
      pgp_packet(p, type);
      buf_cat(key, p);
    }
  nextkey:
    ;
  }
  if (tempbuf)
    buf_free(key);
  buf_free(p);
  buf_free(i);
  buf_free(thisid);
  return (found ? 0 : -1);
}

int pgpdb_append(KEYRING *keydb, BUFFER *p)
{
  assert(keydb->lock);
  buf_cat(keydb->db, p);
  keydb->modified = 1;
  return (0);
}

#define pgp_preferredalgo PGP_ES_RSA

int pgpdb_getkey(int mode, int algo, int *sym, int *mdc, BUFFER *key, BUFFER *userid,
		 BUFFER *founduid, BUFFER *keyid, char *keyring, BUFFER *pass)
{
  KEYRING *r;
  BUFFER *id, *thisid, *thiskey;
  int thisalgo, algofound = -1, needpass = 0;
  int found = 0;

  id = buf_new();
  thisid = buf_new();
  thiskey = buf_new();
  if (keyring)
    r = pgpdb_open(keyring, pass, 0);
  else
    switch (mode) {
    case PK_DECRYPT:
    case PK_SIGN:
      r = pgpdb_open(PGPREMSECRING, NULL, 0);
      break;
    case PK_ENCRYPT:
    case PK_VERIFY:
      r = pgpdb_open(PGPREMPUBRING, NULL, 0);
      if (r != NULL && r->filetype == -1) {
	pgpdb_close(r);
	r = pgpdb_open(PGPREMPUBASC, NULL, 0);
      }
      break;
    default:
      r = NULL;
    }
  if (r == NULL)
    goto end;

  for (;;) {
    /* repeat until success or end of key ring */
    if (pgpdb_getnext(r, thiskey, keyid, userid) == -1)
      break;
    if (keyid) /* pgp_getkey has to chose subkey with given keyid */
      buf_set(thisid, keyid);
    thisalgo = pgp_getkey(mode, algo, sym, mdc, thiskey, thiskey, thisid, founduid,
			  pass);
    if (thisalgo == PGP_PASS)
      needpass = 1;
    if (thisalgo > 0) {
      found++;
      if ((thisalgo == pgp_preferredalgo && algofound != pgp_preferredalgo
	   && algofound > 0)
	  || (thisalgo != pgp_preferredalgo && algofound == pgp_preferredalgo))
	found--; /* ignore the non-preferred algorithm */
      if (found <= 1 || (thisalgo == pgp_preferredalgo &&
			 algofound != pgp_preferredalgo && algofound > 0)) {
	algofound = thisalgo;
	if (key)
	  buf_move(key, thiskey);
	buf_set(id, thisid);
      }
    }
  }
  pgpdb_close(r);
end:
  if (found < 1) {
    if (needpass)
      errlog(DEBUGINFO, "Need passphrase!\n");
    else if (!sym || *sym != PGP_K_IDEA) { /* kludge: try again with 3DES */
      if (userid)
	errlog(NOTICE, "Key %b not found!\n", userid);
      else if (keyid && keyid->length > 7)
	errlog(NOTICE, "Key %02X%02X%02X%02X not found!\n", keyid->data[4],
               keyid->data[5], keyid->data[6], keyid->data[7]);
    }
  }
  if (found > 1) {
    if (userid)
      errlog(WARNING, "Key %b not unique!\n", userid);
    else if (keyid && keyid->length > 7)
      errlog(ERRORMSG, "Key %02X%02X%02X%02X not unique!\n", keyid->data[4],
	     keyid->data[5], keyid->data[6], keyid->data[7]);
    else
      errlog(WARNING, "Key not unique!\n");
  }
  if (found && keyid) /* return ID of found key */
    buf_set(keyid, id);

  buf_free(thiskey);
  buf_free(thisid);
  buf_free(id);
  return (algofound);
}

int pgp_keymgt(int force)
{
  FILE *f = NULL;
  BUFFER *key, *userid, *out, *outkey, *outtxt, *pass;
  KEYRING *keys;
  int err = 0, type = 0;

  key = buf_new();
  out = buf_new();

  userid = buf_new();
  buf_sets(userid, REMAILERNAME);
  pass = buf_new();
  buf_sets(pass, PASSPHRASE);
  outtxt = buf_new();
  outkey = buf_new();

#ifdef USE_RSA
  /* We only want to build RSA keys if we also can do IDEA
   * This is to not lose any mail should users try our RSA key
   * with IDEA.
   */
#ifdef USE_IDEA
  if (force == 2 || (pgpdb_getkey(PK_DECRYPT, PGP_ES_RSA, NULL, NULL, NULL, NULL,
				  NULL, NULL, NULL, pass) < 0))
    pgp_keygen(PGP_ES_RSA, 0, userid, pass, PGPKEY, PGPREMSECRING, 0);

  if (force == 0 && (pgpdb_getkey(PK_ENCRYPT, PGP_ES_RSA, NULL, NULL, NULL, NULL,
				  NULL, NULL, PGPKEY, NULL) < 0))
    goto end;
#endif
#endif
  if (force == 2 || (pgpdb_getkey(PK_DECRYPT, PGP_E_ELG, NULL, NULL, NULL, NULL,
				  NULL, NULL, NULL, pass) < 0))
    pgp_keygen(PGP_E_ELG, 0, userid, pass, PGPKEY, PGPREMSECRING, 0);

  if (force == 0 && (pgpdb_getkey(PK_ENCRYPT, PGP_E_ELG, NULL, NULL, NULL, NULL,
				  NULL, NULL, PGPKEY, NULL) > 0))
    goto end;

  /* write RSA and DSA/ElGamal keys separately to make old PGP
     versions happy */
  err = -1;
  for (type = 0; type < 2; type++) {
    keys = pgpdb_open(PGPREMSECRING, NULL, 0);
    if (keys == NULL)
      goto end;
    while (pgpdb_getnext(keys, key, NULL, userid) != -1) {
      buf_clear(outkey);
      buf_clear(outtxt);
      if (pgp_makepubkey(key, outtxt, outkey, pass,
			 type == 0 ? PGP_ES_RSA : PGP_S_DSA) == 0) {
        err = 0;
	buf_appends(out, "Type Bits/KeyID    Date       User ID\n");
	buf_cat(out, outtxt);
	pgp_armor(outkey, 2);
	buf_nl(out);
	buf_cat(out, outkey);
	buf_nl(out);
      }
    }
    pgpdb_close(keys);
  }

  if (err == 0 && (f = mix_openfile(PGPKEY, "w")) != NULL) {
      buf_write(out, f);
      fclose(f);
    } else
      err = -1;
end:
  buf_free(key);
  buf_free(out);
  buf_free(userid);
  buf_free(pass);
  buf_free(outtxt);
  buf_free(outkey);
  return (err);
}

int pgp_rlist(REMAILER remailer[], int n)
     /* verify that keys are available */
{
  BUFFER *keyring, *p;
  int i, type, pgpkey[MAXREM];

  keyring = buf_new();
  p = buf_new();
  for (i = 1; i < n; i++)
    pgpkey[i] = 0;
  if (pgp_readkeyring(keyring, PGPREMPUBRING) == -1)
    pgp_readkeyring(keyring, PGPREMPUBASC);
  while ((type = pgp_getpacket(keyring, p)) != -1)
    if (type == PGP_USERID)
      for (i = 1; i < n; i++)
	if (remailer[i].flags.pgp && bufifind(p, remailer[i].name))
	  pgpkey[i] = 1;
  for (i = 1; i < n; i++)
    remailer[i].flags.pgp = pgpkey[i];
  buf_free(p);
  buf_free(keyring);
  return (0);
}

#endif /* USE_PGP */
