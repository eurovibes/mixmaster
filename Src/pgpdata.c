/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   OpenPGP data
   $Id: pgpdata.c,v 1.5 2002/07/10 00:40:56 weaselp Exp $ */


#include "mix3.h"
#ifdef USE_PGP
#include "pgp.h"
#include "crypto.h"
#include <assert.h>
#include <time.h>
#include <time.h>

int pgp_keylen(int symalgo)
{
  switch (symalgo) {
  case PGP_K_IDEA:
  case PGP_K_CAST5:
  case PGP_K_BF:
    return (16);
  case PGP_K_3DES:
    return (24);
  default:
    return (-1);
  }
}

int mpi_get(BUFFER *b, BUFFER *mpi)
{
  int l;

  l = buf_geti(b);
  buf_clear(mpi);

  if (l <= 0 || b->ptr + (l + 7) / 8 > b->length)
    return (-1);
  buf_get(b, mpi, (l + 7) / 8);
  return (l);
}

  int l;
  int i;

int mpi_bitcount(BUFFER *mpi)
{
  int l = mpi->length * 8;
  for (i = 7; i >= 0; i--)
    if (((mpi->data[0] >> i) & 1) == 1) {
      l -= 7 - i;
      break;
    }
  return l;
}

int mpi_put(BUFFER *b, BUFFER *mpi)
{
  buf_appendi(b, mpi_bitcount(mpi));
  buf_cat(b, mpi);
  return (0);
}

int skcrypt(BUFFER *data, int skalgo, BUFFER *key, BUFFER *iv, int enc)
{
  switch (skalgo) {
  case 0:
    return (0);
#ifdef USE_IDEA
  case PGP_K_IDEA:
    return (buf_ideacrypt(data, key, iv, enc));
#endif
  case PGP_K_3DES:
    return (buf_3descrypt(data, key, iv, enc));
  case PGP_K_BF:
    return (buf_bfcrypt(data, key, iv, enc));
  case PGP_K_CAST5:
    return (buf_castcrypt(data, key, iv, enc));
  default:
    return (-1);
  }
}

int pgp_csum(BUFFER *key, int start)
{
  int i, csum = 0;
  for (i = start; i < key->length; i++)
    csum = (csum + key->data[i]) % 65536;
  return (csum);
}

#ifdef USE_RSA
int pgp_rsa(BUFFER *in, BUFFER *k, int mode)
{
  BUFFER *mpi, *out;
  int err = -1;
  RSA *key;

  assert(mode == PK_ENCRYPT || mode == PK_VERIFY || mode == PK_DECRYPT
	 || mode == PK_SIGN);
  key = RSA_new();
  out = buf_new();
  mpi = buf_new();

  mpi_get(k, mpi);
  key->n = BN_bin2bn(mpi->data, mpi->length, NULL);

  if (mpi_get(k, mpi) < 0)
    goto end;
  key->e = BN_bin2bn(mpi->data, mpi->length, NULL);

  if (mode == PK_DECRYPT || mode == PK_SIGN) {
    if (mpi_get(k, mpi) < 0)
      goto end;
    key->d = BN_bin2bn(mpi->data, mpi->length, NULL);

#if 1
    /* compute auxiluary parameters */
    mpi_get(k, mpi);		/* PGP'p is SSLeay's q */
    key->q = BN_bin2bn(mpi->data, mpi->length, NULL);

    mpi_get(k, mpi);
    key->p = BN_bin2bn(mpi->data, mpi->length, NULL);

    if (mpi_get(k, mpi) < 0)
      goto end;
    key->iqmp = BN_bin2bn(mpi->data, mpi->length, NULL);

    {
      BIGNUM *i;
      BN_CTX *ctx;

      ctx = BN_CTX_new();
      i = BN_new();
      key->dmp1 = BN_new();
      key->dmq1 = BN_new();

      BN_sub(i, key->p, BN_value_one());
      BN_mod(key->dmp1, key->d, i, ctx);

      BN_sub(i, key->q, BN_value_one());
      BN_mod(key->dmq1, key->d, i, ctx);

      BN_free(i);
    }
#endif
  }
  buf_prepare(out, RSA_size(key));

  switch (mode) {
  case PK_ENCRYPT:
    out->length = RSA_public_encrypt(in->length, in->data, out->data, key,
				     RSA_PKCS1_PADDING);
    break;
  case PK_VERIFY:
    out->length = RSA_public_decrypt(in->length, in->data, out->data, key,
				     RSA_PKCS1_PADDING);
    break;
  case PK_SIGN:
    out->length = RSA_private_encrypt(in->length, in->data, out->data, key,
				      RSA_PKCS1_PADDING);
    break;
  case PK_DECRYPT:
    out->length = RSA_private_decrypt(in->length, in->data, out->data, key,
				      RSA_PKCS1_PADDING);
    break;
  }
  if (out->length == -1)
    err = -1, out->length = 0;
  else
    err = 0;

  buf_move(in, out);
end:
  RSA_free(key);
  buf_free(out);
  buf_free(mpi);
  return (err);
}
#endif

/* Contrary to RFC 2440, old PGP versions use this for clearsign only.
 * If the text is included in the OpenPGP message, the application will
 * typically provide the text in the proper format (whatever that is);
 * we use "canonic" format so everybody will be able to read our messages.
 * In clearsigned messages, trailing whitespace is always ignored.
 * Detached signatures are the problematic case. For PGP/MIME, we always
 * escape trailing whitespace as quoted-printable.
 */
void pgp_sigcanonic(BUFFER *msg)
{
  BUFFER *line, *out;

  out = buf_new();
  line = buf_new();

  while (buf_getline(msg, line) != -1) {
    while (line->length > 0 && (line->data[line->length - 1] == ' '
#if 0
				|| line->data[line->length - 1] == '\t'
#endif
	))
      line->length--;
    line->data[line->length] = '\0';
    buf_cat(out, line);
    buf_appends(out, "\r\n");
  }
  buf_move(msg, out);
  buf_free(out);
  buf_free(line);
}

static void mpi_bnput(BUFFER *o, BIGNUM *i)
{
  BUFFER *b;

  b = buf_new();
  buf_prepare(b, BN_num_bytes(i));
  b->length = BN_bn2bin(i, b->data);
  mpi_put(o, b);
  buf_free(b);
}

static void mpi_bnputenc(BUFFER *o, BIGNUM *i, int ska, BUFFER *key,
			 BUFFER *iv)
{
  BUFFER *b;

  b = buf_new();
  buf_prepare(b, BN_num_bytes(i));
  b->length = BN_bn2bin(i, b->data);
  buf_appendi(o, mpi_bitcount(b));
  if (key && key->length)
    skcrypt(b, ska, key, iv, ENCRYPT);
  buf_cat(o, b);
  buf_free(b);
}

static int getski(BUFFER *p, BUFFER *pass, BUFFER *key, BUFFER *iv)
{
  int skalgo;
  BUFFER *salt, *temp;

  if (!pass)
    return(-1);

  salt = buf_new();
  temp = buf_new();

  skalgo = buf_getc(p);
  switch (skalgo) {
  case 0:
    /* none */
    goto end;
  case 255:
    /* S2K specifier */
    skalgo = pgp_getsk(p, pass, key);
    break;
  default:
    /* simple */
    digest_md5(pass, key);
    break;
  }

  buf_get(p, iv, 8);

 end:
  buf_free(salt);
  buf_free(temp);
  return (skalgo);
}

static void makeski(BUFFER *secret, BUFFER *pass, int remail)
{
  BUFFER *out, *key, *iv;
  out = buf_new();
  key = buf_new();
  iv = buf_new();
  if (pass == NULL || pass->length == 0 || remail == 2) {
    buf_appendc(out, 0);
    buf_cat(out, secret);
  } else {
    buf_appendc(out, 255);
    pgp_makesk(out, key, PGP_K_CAST5, 3, PGP_H_SHA1, pass);
    buf_setrnd(iv, 8);
    buf_cat(out, iv);
    skcrypt(secret, PGP_K_CAST5, key, iv, 1);
    buf_cat(out, secret);
  }
  buf_move(secret, out);
  buf_free(out);
  buf_free(key);
  buf_free(iv);
}

int pgp_nummpi(int algo)
{
  switch (algo) {
   case PGP_ES_RSA:
    return (2);
   case PGP_S_DSA:
    return (4);
   case PGP_E_ELG:
    return (3);
   default:
    return (0);
  }
}

int pgp_numsecmpi(int algo)
{
  switch (algo) {
   case PGP_ES_RSA:
    return (4);
   case PGP_S_DSA:
    return (1);
   case PGP_E_ELG:
    return (1);
   default:
    return (0);
  }
}

/* store key's ID in keyid */
int pgp_keyid(BUFFER *key, BUFFER *keyid)
{
  BUFFER *i, *k;
  int version, algo, j, ptr;

  i = buf_new();
  k = buf_new();

  ptr = key->ptr;
  key->ptr = 0;
  switch (version = buf_getc(key)) {
  case 2:
  case 3:
    buf_getl(key);
    buf_geti(key);
    buf_getc(key);
    mpi_get(key, i);
    break;
  case 4:
    buf_appendc(k, version);
    buf_appendl(k, buf_getl(key));
    algo = buf_getc(key);
    buf_appendc(k, algo);
    if (pgp_nummpi(algo) == 0)
      buf_rest(k, key); /* works for public keys only */
    else
      for (j = 0; j < pgp_nummpi(algo); j++) {
	mpi_get(key, i);
	mpi_put(k, i);
      }
    buf_clear(i);
    buf_appendc(i, 0x99);
    buf_appendi(i, k->length);
    buf_cat(i, k);
    digest_sha1(i, i);
    break;
  }
  buf_clear(keyid);
  buf_append(keyid, i->data + i->length - 8, 8);
  buf_free(i);
  buf_free(k);
  key->ptr = ptr;
  return(0);
}

static int pgp_iskeyid(BUFFER *key, BUFFER *keyid)
{
  BUFFER *thisid;
  int ret;

  thisid = buf_new();
  pgp_keyid(key, thisid);
  ret = buf_eq(keyid, thisid);
  buf_free(thisid);
  return(ret);
}

int pgp_getkey(int mode, int algo, int *psym, BUFFER *keypacket, BUFFER *key,
	       BUFFER *keyid, BUFFER *userid, BUFFER *pass)
{
  int tempbuf = 0;
  int keytype = -1, type, j;
  int thisalgo, version, skalgo;
  int needsym = 0, symfound = 0;
  BUFFER *p1, *iv, *sk, *i, *thiskeyid;
  int csstart;

  p1 = buf_new();
  i = buf_new();
  iv = buf_new();
  sk = buf_new();
  thiskeyid = buf_new();
  if (psym)
    needsym = *psym;
  if (keypacket == key) {
    key = buf_new();
    tempbuf = 1;
  }
  if (userid)
    buf_clear(userid);

  while ((type = pgp_getpacket(keypacket, p1)) > 0) {
    switch (type) {
    case PGP_SIG:
      /* it is assumed that only valid keys have been imported */
      if (buf_getc(p1) == 4) {
	if (buf_getc(p1) == PGP_SIG_CERT) {
	  buf_getc(p1);
	  buf_getc(p1);
	  j = buf_geti(p1);
	  j += p1->ptr;
	  while (p1->ptr < j) {
	    int len, type, a;
	    len = buf_getc(p1);
	    if (len > 192 && len < 255)
	      len = (len - 192) * 256 + buf_getc(p1) + 192;
	    else if (len == 255)
	      len = buf_getl(p1);
	    type = buf_getc(p1);
	    if (len)
	      buf_get(p1, i, len);
	    else
	      buf_clear(i);
	    if (type == PGP_SUB_PSYMMETRIC)
	      while ((a = buf_getc(i)) != -1)
		if ((a == PGP_K_3DES || a == PGP_K_CAST5
#ifdef USE_IDEA
		     || a == PGP_K_IDEA
#endif
		     ) && (a == needsym || needsym == 0)) {
		  symfound = a;
		  break;
		}
	  }
	}
      }
      break;
    case PGP_USERID:
      if (userid)
	buf_move(userid, p1);
      break;
    case PGP_PUBSUBKEY:
    case PGP_SECSUBKEY:
      if (keytype != -1 && (mode == PK_SIGN || mode == PK_VERIFY))
	continue;
    case PGP_PUBKEY:
    case PGP_SECKEY:
      if ((type == PGP_PUBKEY || type == PGP_PUBSUBKEY) &&
	  (mode == PK_DECRYPT || mode == PK_SIGN))
	continue;
      keytype = type;
      version = buf_getc(p1);
      switch (version) {
      case 2:
      case 3:
	buf_getl(p1);		/* created */
	buf_geti(p1);		/* valid */
	thisalgo = buf_getc(p1);
	if (thisalgo != PGP_ES_RSA) {
	  keytype = -1;
	  goto end;
	}
	symfound = PGP_K_IDEA;
	break;
      case 4:
	buf_appendc(key, version);
	buf_appendl(key, buf_getl(p1));
	thisalgo = buf_getc(p1);
	buf_appendc(key, thisalgo);
	if (symfound == 0)
	  symfound = PGP_K_3DES; /* default algorithm */
	break;
      default:
	keytype = -1;
	goto end;
      }
      if (algo != PGP_ANY && thisalgo != algo) {
	keytype = -1;
	continue;
      }
      if (keyid && keyid->length && !pgp_iskeyid(p1, keyid))
	continue;
      pgp_keyid(p1, thiskeyid);
      if (key) {
	buf_clear(key);
	for (j = 0; j < pgp_nummpi(thisalgo); j++) {
	  if (mpi_get(p1, i) == -1)
	    goto end;
	  mpi_put(key, i);
	}
	if (keytype == PGP_SECKEY || keytype == PGP_SECSUBKEY) {
	  csstart = key->length;
	  skalgo = getski(p1, pass, sk, iv);
	  switch (version) {
	   case 2:
	   case 3:
	    for (j = 0; j < pgp_numsecmpi(thisalgo); j++) {
	      unsigned char lastb[8];
	      if (mpi_get(p1, i) == -1) {
		keytype = -1;
		goto end;
	      }
	      if (j)
		{buf_clear(iv); buf_append(iv, lastb, 8);} // memcpy(iv->data, lastb, 8);
	      memcpy(lastb, i->data+i->length-8, 8);
	      skcrypt(i, skalgo, sk, iv, DECRYPT);
	      mpi_put(key, i);
	    }
	    break;
	   case 4:
	    buf_clear(i);
	    buf_rest(i, p1);
	    skcrypt(i, skalgo, sk, iv, DECRYPT);
	    buf_move(p1, i);
	    for (j = 0; j < pgp_numsecmpi(thisalgo); j++) {
	      if (mpi_get(p1, i) == -1) {
		keytype = PGP_PASS;
		goto end;
	      }
	      mpi_put(key, i);
	    }
	    break;
	  }
	  if (pgp_csum(key, csstart) != buf_geti(p1)) {
	    keytype = PGP_PASS;
	    goto end;
	  }
	}
      }
      break;
     default:
      /* ignore trust packets etc */
      break;
    }
  }
 end:
  if (keyid) buf_set(keyid, thiskeyid);
  if (tempbuf) {
    buf_move(keypacket, key);
    buf_free(key);
  }
  buf_free(p1);
  buf_free(i);
  buf_free(iv);
  buf_free(sk);
  buf_free(thiskeyid);
#ifndef USE_RSA
  if (thisalgo == PGP_ES_RSA)
    keytype = -1;
#endif
  if (needsym > 0 && symfound != needsym)
    keytype = -1;
  else if (psym && *psym == 0)
    *psym = symfound;

  return (keytype <= 0 ? keytype : thisalgo);
}

int pgp_makepkpacket(int type, BUFFER *p, BUFFER *outtxt, BUFFER *out,
		     BUFFER *key, BUFFER *pass, time_t *created)
{
  BUFFER *i, *id;
  char txt[LINELEN], algoid;
  int version, algo, valid = 0, err = 0;
  int len, j;
  struct tm *tc;

  i = buf_new();
  id = buf_new();

  version = buf_getc(p);
  buf_clear(key);
  switch (version) {
  case 2:
  case 3:
    *created = buf_getl(p);
    valid = buf_geti(p);
    algo = buf_getc(p);
    if (algo != PGP_ES_RSA)
      return(-1);
    break;
  case 4:
    *created = buf_getl(p);
    algo = buf_getc(p);
    break;
  default:
    return(-1);
  }

  switch (version) {
  case 2:
  case 3:
    buf_appendc(key, version);
    buf_appendl(key, *created);
    buf_appendi(key, valid);
    buf_appendc(key, algo);
    break;
  case 4:
    buf_appendc(key, version);
    buf_appendl(key, *created);
    buf_appendc(key, algo);
    break;
  }

  pgp_keyid(p, id);
  len = mpi_get(p, i);
  mpi_put(key, i);
  for (j = 1; j < pgp_nummpi(algo); j++) {
    if (mpi_get(p, i) == -1) {
      err = -1;
      goto end;
    }
    mpi_put(key, i);
  }
  pgp_packet(key, type);
  buf_cat(out, key);

  if (outtxt != NULL) {
    switch(algo) {
     case PGP_ES_RSA:
      algoid = 'R';
      break;
     case PGP_S_DSA:
      algoid = 'D';
      break;
     case PGP_E_ELG:
      algoid = 'g';
      break;
     default:
      algoid = '?';
    }
    buf_appendf(outtxt, "%s %4d%c/%02X%02X%02X%02X ", type == PGP_PUBSUBKEY ?
		"sub" : "pub", len, algoid,
		id->data[4], id->data[5], id->data[6], id->data[7]);
    tc = localtime(created);
    strftime(txt, LINELEN, "%Y/%m/%d ", tc);
    buf_appends(outtxt, txt);
  }
 end:
  buf_free(i);
  buf_free(id);
  return(err == 0 ? algo : err);
}

int pgp_makepubkey(BUFFER *keypacket, BUFFER *outtxt, BUFFER *out,
		   BUFFER *pass, int keyalgo)
{
  BUFFER *p, *pubkey, *seckey, *subkey, *sig, *tmp;
  int err = -1, type, thisalgo;
  time_t created;

  p = buf_new();
  seckey = buf_new();
  pubkey = buf_new();
  subkey = buf_new();
  sig = buf_new();
  tmp = buf_new();

  buf_set(seckey, keypacket);
  type = pgp_getpacket(keypacket, p);
  if (type != PGP_SECKEY)
    goto end;

  thisalgo = pgp_makepkpacket(PGP_PUBKEY, p, outtxt, tmp, pubkey, pass,
			      &created);
  if (thisalgo == -1 || (keyalgo != 0 && keyalgo != thisalgo))
    goto end;
  buf_cat(out, tmp);

  while ((type = pgp_getpacket(keypacket, p)) > 0) {
    if (type == PGP_SECSUBKEY) {
      if (pgp_makepkpacket(PGP_PUBSUBKEY, p, outtxt, out, subkey, pass,
			   &created) == -1)
	goto end;
      if (pgp_sign(pubkey, subkey, sig, NULL, pass, PGP_SIG_BINDSUBKEY, 0,
		   created, 0, seckey, NULL) != -1)
	buf_cat(out, sig);
      if (outtxt)
	buf_nl(outtxt);
    } else if (type == PGP_USERID) {
      if (outtxt != NULL) {
	buf_cat(outtxt, p);
	buf_nl(outtxt);
      }
      pgp_packet(p, PGP_USERID);
      err = pgp_sign(pubkey, p, sig, NULL, pass, PGP_SIG_CERT, 1, created, 0,
		     seckey, NULL);
      buf_cat(out, p);
      if (err == 0)
	buf_cat(out, sig);
    } else if (type == PGP_PUBKEY || type == PGP_SECKEY)
      break;
  }
end:
  buf_free(pubkey);
  buf_free(seckey);
  buf_free(subkey);
  buf_free(sig);
  buf_free(p);
  buf_free(tmp);
  return (err);
}

#ifdef USE_RSA
int pgp_rsakeygen(int bits, BUFFER *userid, BUFFER *pass, char *pubring,
	       char *secring, int remail)
     /* remail==2: encrypt the secring */
{
  RSA *k;
  KEYRING *keydb;
  BUFFER *pkey, *skey;
  BUFFER *dk, *sig, *iv, *p;
  long now;
  int skalgo = 0;
  int err = 0;

  pkey = buf_new();
  skey = buf_new();
  iv = buf_new();
  dk = buf_new();
  p = buf_new();
  sig = buf_new();

  errlog(NOTICE, "Generating OpenPGP RSA key.\n");
  k = RSA_generate_key(bits == 0 ? 1024 : bits, 3, NULL, NULL);
  if (k == NULL) {
    err = -1;
    goto end;
  }
  now = time(NULL);
  if (remail)			/* fake time in nym keys */
    now -= rnd_number(4 * 24 * 60 * 60);

  buf_appendc(skey, 3);
  buf_appendl(skey, now);
  buf_appendi(skey, 0);
  buf_appendc(skey, PGP_ES_RSA);
  mpi_bnput(skey, k->n);
  mpi_bnput(skey, k->e);

#ifdef USE_IDEA
  if (pass != NULL && pass->length > 0 && remail != 2) {
    skalgo = PGP_K_IDEA;
    digest_md5(pass, dk);
    buf_setrnd(iv, 8);
    buf_appendc(skey, skalgo);
    buf_cat(skey, iv);
  }
  else
#endif
    buf_appendc(skey, 0);

  mpi_bnputenc(skey, k->d, skalgo, dk, iv);
  mpi_bnputenc(skey, k->q, skalgo, dk, iv);
  mpi_bnputenc(skey, k->p, skalgo, dk, iv);
  mpi_bnputenc(skey, k->iqmp, skalgo, dk, iv);

  buf_clear(p);
  mpi_bnput(p, k->d);
  mpi_bnput(p, k->q);
  mpi_bnput(p, k->p);
  mpi_bnput(p, k->iqmp);
  buf_appendi(skey, pgp_csum(p, 0));

  pgp_packet(skey, PGP_SECKEY);
  buf_set(p, userid);
  pgp_packet(p, PGP_USERID);
  buf_cat(skey, p);

  if (secring == NULL)
    secring = PGPREMSECRING;
  keydb = pgpdb_open(secring, remail == 2 ? pass : NULL, 1);
  if (keydb == NULL) {
    err = -1;
    goto end;
  }
  if (keydb->filetype == -1)
    keydb->filetype = 0;
  pgpdb_append(keydb, skey);
  pgpdb_close(keydb);

  if (pubring != NULL) {
    if (pgp_makepubkey(skey, NULL, pkey, pass, 0) == -1)
      goto end;
    keydb = pgpdb_open(pubring, NULL, 1);
    if (keydb == NULL)
      goto end;
    if (keydb->filetype == -1)
      keydb->filetype = ARMORED;
    pgpdb_append(keydb, pkey);
    pgpdb_close(keydb);
  }
end:
  RSA_free(k);
  buf_free(pkey);
  buf_free(skey);
  buf_free(iv);
  buf_free(dk);
  buf_free(p);
  buf_free(sig);
  return (err);
}
#endif

#define begin_param "-----BEGIN PUBLIC PARAMETER BLOCK-----"
#define end_param "-----END PUBLIC PARAMETER BLOCK-----"

static void *params(int dsa, int bits)
{
  DSA *k = NULL;
  DH *d = NULL;
  FILE *f;
  BUFFER *p, *n;
  char line[LINELEN];
  byte b[1024];
  int m, l;

  if (bits == 0)
    bits = 1024;
  if (dsa && bits > 1024)
    bits = 1024;

  p = buf_new();
  n = buf_new();
  f = mix_openfile(dsa ? DSAPARAMS : DHPARAMS, "r");
  if (f != NULL) {
    for (;;) {
      if (fgets(line, sizeof(line), f) == NULL)
	break;
      if (strleft(line, begin_param)) {
	if (fgets(line, sizeof(line), f) == NULL)
	  break;
	m = 0;
	sscanf(line, "%d", &m);
	if (bits == m) {
	  buf_clear(p);
	  while (fgets(line, sizeof(line), f) != NULL) {
	    if (strleft(line, end_param)) {
	      decode(p, p);
	      if (dsa) {
		k = DSA_new();
		l = buf_geti(p);
		buf_get(p, n, l);
		k->p = BN_bin2bn(n->data, n->length, NULL);
		l = buf_geti(p);
		buf_get(p, n, l);
		k->q = BN_bin2bn(n->data, n->length, NULL);
		l = buf_geti(p);
		buf_get(p, n, l);
		k->g = BN_bin2bn(n->data, n->length, NULL);
	      } else {
		d = DH_new();
		l = buf_geti(p);
		buf_get(p, n, l);
		d->p = BN_bin2bn(n->data, n->length, NULL);
		l = buf_geti(p);
		buf_get(p, n, l);
		d->g = BN_bin2bn(n->data, n->length, NULL);
	      }
	      break;
	    }
	    buf_appends(p, line);
	  }
	}
      }
    }
    fclose(f);
  }

  buf_free(p);
  buf_free(n);

  if (dsa) {
    if (k == NULL) {
      errlog(NOTICE, "Generating DSA parameters.\n");
      k = DSA_generate_parameters(bits, NULL, 0, NULL, NULL, NULL, NULL);
      p = buf_new();
      l = BN_bn2bin(k->p, b);
      buf_appendi(p, l);
      buf_append(p, b, l);
      l = BN_bn2bin(k->q, b);
      buf_appendi(p, l);
      buf_append(p, b, l);
      l = BN_bn2bin(k->g, b);
      buf_appendi(p, l);
      buf_append(p, b, l);
      encode(p, 64);
      f = mix_openfile(DSAPARAMS, "a");
      fprintf(f, "%s\n%d\n", begin_param, bits);
      buf_write(p, f);
      fprintf(f, "%s\n", end_param);
      fclose(f);
      buf_free(p);
    }
    return (k);
  } else {
    if (d == NULL) {
      errlog(NOTICE, "Generating DH parameters. (This may take a long time!)\n");
      d = DH_generate_parameters(bits, DH_GENERATOR_5, NULL, NULL);
      p = buf_new();
      l = BN_bn2bin(d->p, b);
      buf_appendi(p, l);
      buf_append(p, b, l);
      l = BN_bn2bin(d->g, b);
      buf_appendi(p, l);
      buf_append(p, b, l);
      encode(p, 64);
      f = mix_openfile(DHPARAMS, "a");
      fprintf(f, "%s\n%d\n", begin_param, bits);
      buf_write(p, f);
      fprintf(f, "%s\n", end_param);
      fclose(f);
      buf_free(p);
    }
    return (d);
  }
}

int pgp_dhkeygen(int bits, BUFFER *userid, BUFFER *pass, char *pubring,
	       char *secring, int remail)
     /* remail==2: encrypt the secring */
{
  DSA *s;
  DH *e;
  KEYRING *keydb;
  BUFFER *pkey, *skey, *subkey, *secret;
  BUFFER *dk, *sig, *iv, *p;
  long now;
  int err = 0;

  pkey = buf_new();
  skey = buf_new();
  subkey = buf_new();
  iv = buf_new();
  dk = buf_new();
  p = buf_new();
  sig = buf_new();
  secret = buf_new();

  s = params(1, bits);
  errlog(NOTICE, "Generating OpenPGP DSA key.\n");
  if (s == NULL || DSA_generate_key(s) != 1) {
    err = -1;
    goto end;
  }
  e = params(0, bits);
  errlog(NOTICE, "Generating OpenPGP ElGamal key.\n");
  if (e == NULL || DH_generate_key(e) != 1) {
    err = -1;
    goto end;
  }

  now = time(NULL);
  if (remail)			/* fake time in nym keys */
    now -= rnd_number(4 * 24 * 60 * 60);

  /* DSA key */
  buf_setc(skey, 4);
  buf_appendl(skey, now);
  buf_appendc(skey, PGP_S_DSA);
  mpi_bnput(skey, s->p);
  mpi_bnput(skey, s->q);
  mpi_bnput(skey, s->g);
  mpi_bnput(skey, s->pub_key);

  mpi_bnput(secret, s->priv_key);
  buf_appendi(secret, pgp_csum(secret, 0));
  makeski(secret, pass, remail);
  buf_cat(skey, secret);
  pgp_packet(skey, PGP_SECKEY);

  /* ElGamal key */
  buf_setc(subkey, 4);
  buf_appendl(subkey, now);
  buf_appendc(subkey, PGP_E_ELG);
  mpi_bnput(subkey, e->p);
  mpi_bnput(subkey, e->g);
  mpi_bnput(subkey, e->pub_key);

  buf_clear(secret);
  mpi_bnput(secret, e->priv_key);
  buf_appendi(secret, pgp_csum(secret, 0));
  makeski(secret, pass, remail);
  buf_cat(subkey, secret);

  buf_set(p, userid);
  pgp_packet(p, PGP_USERID);
  buf_cat(skey, p);

  pgp_packet(subkey, PGP_SECSUBKEY);
  buf_cat(skey, subkey);

  if (secring == NULL)
    secring = PGPREMSECRING;
  keydb = pgpdb_open(secring, remail == 2 ? pass : NULL, 1);
  if (keydb == NULL) {
    err = -1;
    goto end;
  }
  if (keydb->filetype == -1)
    keydb->filetype = 0;
  pgpdb_append(keydb, skey);
  pgpdb_close(keydb);

  if (pubring != NULL) {
    pgp_makepubkey(skey, NULL, pkey, pass, 0);
    keydb = pgpdb_open(pubring, NULL, 1);
    if (keydb == NULL)
      goto end;
    if (keydb->filetype == -1)
      keydb->filetype = ARMORED;
    pgpdb_append(keydb, pkey);
    pgpdb_close(keydb);
  }
end:
  buf_free(pkey);
  buf_free(skey);
  buf_free(subkey);
  buf_free(iv);
  buf_free(dk);
  buf_free(p);
  buf_free(sig);
  buf_free(secret);
  return (err);
}

int pgp_dsasign(BUFFER *data, BUFFER *key, BUFFER *out)
{
  BUFFER *mpi, *b;
  DSA *d;
  DSA_SIG *sig = NULL;

  d = DSA_new();
  b = buf_new();
  mpi = buf_new();
  mpi_get(key, mpi);
  d->p = BN_bin2bn(mpi->data, mpi->length, NULL);
  mpi_get(key, mpi);
  d->q = BN_bin2bn(mpi->data, mpi->length, NULL);
  mpi_get(key, mpi);
  d->g = BN_bin2bn(mpi->data, mpi->length, NULL);
  mpi_get(key, mpi);
  d->pub_key = BN_bin2bn(mpi->data, mpi->length, NULL);
  if (mpi_get(key, mpi) == -1) {
    goto end;
  }
  d->priv_key = BN_bin2bn(mpi->data, mpi->length, NULL);

  sig = DSA_do_sign(data->data, data->length, d);
  if (sig) {
    buf_prepare(b, BN_num_bytes(sig->r));
    b->length = BN_bn2bin(sig->r, b->data);
    mpi_put(out, b);
    b->length = BN_bn2bin(sig->s, b->data);
    mpi_put(out, b);
  }
 end:
  buf_free(mpi);
  buf_free(b);
  DSA_SIG_free(sig);
  DSA_free(d);
  return(sig ? 0 : -1);
}

int pgp_dosign(int algo, BUFFER *data, BUFFER *key)
{
  int err;
  BUFFER *out, *r, *s;
  
  out = buf_new();
  r = buf_new();
  s = buf_new();
  switch (algo) {
#ifdef USE_RSA
   case PGP_ES_RSA:
    err = pgp_rsa(data, key, PK_SIGN);
    if (err == 0)
      mpi_put(out, data);
    break;
#endif
   case PGP_S_DSA:
    err = pgp_dsasign(data, key, out);
    break;
   default:
    errlog(NOTICE, "Unknown encryption algorithm!\n");
    return (-1);
  }
  if (err == -1)
    errlog(ERRORMSG, "Signing operation failed!\n");

  buf_move(data, out);
  buf_free(out);
  buf_free(r);
  buf_free(s);
  return (err);
}

int pgp_elgdecrypt(BUFFER *in, BUFFER *key)
{
  BIGNUM *a, *b, *c, *p, *g, *x;
  BN_CTX *ctx;
  BUFFER *i;
  int err = -1;

  i = buf_new();
  ctx = BN_CTX_new();
  if (ctx == NULL) goto end;  
  mpi_get(key, i);
  p = BN_bin2bn(i->data, i->length, NULL);
  mpi_get(key, i);
  g = BN_bin2bn(i->data, i->length, NULL);
  mpi_get(key, i); /* y */
  mpi_get(key, i);
  x = BN_bin2bn(i->data, i->length, NULL);
  mpi_get(in, i);
  a = BN_bin2bn(i->data, i->length, NULL);
  if (mpi_get(in, i) == -1)
    goto e1;
  b = BN_bin2bn(i->data, i->length, NULL);
  c = BN_new();

  if (BN_mod_exp(c, a, x, p, ctx) == 0) goto end;
  if (BN_mod_inverse(a, c, p, ctx) == 0) goto end;
  if (BN_mod_mul(c, a, b, p, ctx) == 0) goto end;

  buf_prepare(i, BN_num_bytes(c));
  i->length = BN_bn2bin(c, i->data);

  buf_prepare(in, BN_num_bytes(c));
  in->length = RSA_padding_check_PKCS1_type_2(in->data, in->length, i->data,
					       i->length, i->length + 1);
  if (in->length <= 0)
    in->length = 0;
  else
    err = 0;

 end:
  BN_free(b);
  BN_free(c);
 e1:
  buf_free(i);
  BN_free(a);
  BN_free(p);
  BN_free(g);
  BN_clear_free(x);
  BN_CTX_free(ctx);

  return (err);
}

int pgp_elgencrypt(BUFFER *in, BUFFER *key)
{
  BIGNUM *m, *k, *a, *b, *c, *p, *g, *y;
  BN_CTX *ctx;
  BUFFER *i;
  int err = -1;
  
  i = buf_new();
  ctx = BN_CTX_new();
  if (ctx == NULL) goto end;  
  mpi_get(key, i);
  p = BN_bin2bn(i->data, i->length, NULL);
  mpi_get(key, i);
  g = BN_bin2bn(i->data, i->length, NULL);
  if (mpi_get(key, i) == -1)
    goto e1;
  y = BN_bin2bn(i->data, i->length, NULL);

  buf_prepare(i, BN_num_bytes(p));
  if (RSA_padding_add_PKCS1_type_2(i->data, i->length, in->data, in->length)
      != 1)
    goto end;
  m = BN_bin2bn(i->data, i->length, NULL);

  k = BN_new();
  BN_rand(k, BN_num_bits(p), 0, 0);

  a = BN_new();
  b = BN_new();
  c = BN_new();

  if (BN_mod_exp(a, g, k, p, ctx) == 0) goto end;
  if (BN_mod_exp(c, y, k, p, ctx) == 0) goto end;
  if (BN_mod_mul(b, m, c, p, ctx) == 0) goto end;

  buf_clear(in);
  i->length = BN_bn2bin(a, i->data);
  mpi_put(in, i);
  i->length = BN_bn2bin(b, i->data);
  mpi_put(in, i);
  
  err = 0;

  BN_free(a);
  BN_free(b);
  BN_free(c);
  BN_free(m);
e1:
  buf_free(i);
  BN_free(p);
  BN_free(g);
  BN_free(y);
  BN_CTX_free(ctx);
 end:

  return (err);
}

#endif /* USE_PGP */
