/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Create OpenPGP packets
   $Id: pgpcreat.c,v 1.2 2001/11/06 23:41:58 rabbi Exp $ */


#include "mix3.h"
#ifdef USE_PGP
#include "pgp.h"
#include "crypto.h"
#include <assert.h>
#include <time.h>
#include <time.h>

int pgp_packet(BUFFER *in, int type)
{
  int ctb;
  BUFFER *out;

  out = buf_new();
  ctb = 128 + (type << 2);
  if (in->length < 256 && type != PGP_PUBKEY && type != PGP_SECKEY &&
      type != PGP_SIG && type != PGP_PUBSUBKEY && type != PGP_SECSUBKEY
#ifdef MIMIC
      && type != PGP_ENCRYPTED
#endif
    ) {
    buf_setc(out, ctb);
    buf_appendc(out, in->length);
  }
#ifndef MIMIC
  else if (in->length < 65536)
#else
  else if ((type == PGP_PUBKEY || type == PGP_SECKEY || type == PGP_SIG
	   || type == PGP_SESKEY || type == PGP_PUBSUBKEY ||
	   type == PGP_SECSUBKEY) && in->length < 65536)
#endif
  {
    buf_appendc(out, ctb | 1);
    buf_appendi(out, in->length);
  } else {
    buf_appendc(out, ctb | 2);
    buf_appendl(out, in->length);
  }
  buf_cat(out, in);
  buf_move(in, out);
  buf_free(out);
  return (0);
}

int pgp_subpacket(BUFFER *in, int type)
{
  BUFFER *out;
  int len;

  out = buf_new();
  len = in->length + 1;
  if (len < 192)
    buf_setc(out, len);
  else {
    buf_setc(out, 255);
    buf_appendl(out, len);
  }
  buf_appendc(out, type);
  buf_cat(out, in);
  buf_move(in, out);
  buf_free(out);
  return (0);
}

int pgp_packet3(BUFFER *in, int type)
{
#ifdef MIMIC
  int ctb;
  BUFFER *out;

  out = buf_new();
  ctb = 128 + (type << 2);
  buf_setc(out, ctb | 3);
  buf_cat(out, in);
  buf_move(in, out);
  buf_free(out);
  return (0);
#else
  return pgp_packet(in, type);
#endif
}

#ifdef USE_IDEA
static int pgp_ideaencrypt(BUFFER *in, BUFFER *out, BUFFER *key)
{
  byte iv[8];
  int i, n = 0;
  IDEA_KEY_SCHEDULE ks;

  assert(key->length == 17);

  for (i = 0; i < 8; i++)
    iv[i] = 0;

  idea_set_encrypt_key(key->data + 1, &ks);

  rnd_bytes(out->data, 8);
  out->data[8] = out->data[6], out->data[9] = out->data[7];
  n = 0;
  idea_cfb64_encrypt(out->data, out->data, 10, &ks, iv, &n, IDEA_ENCRYPT);
  iv[6] = iv[0], iv[7] = iv[1];
  memcpy(iv, out->data + 2, 6);
  n = 0;
  idea_cfb64_encrypt(in->data, out->data + 10, in->length, &ks, iv, &n,
		     IDEA_ENCRYPT);
  return (0);
}
#endif

static int pgp_3desencrypt(BUFFER *in, BUFFER *out, BUFFER *key)
{
  des_cblock iv;
  int i, n = 0;
  des_key_schedule ks1;
  des_key_schedule ks2;
  des_key_schedule ks3;

  assert(key->length == 25);

  for (i = 0; i < 8; i++)
    iv[i] = 0;

  des_set_key((const_des_cblock *) (key->data + 1), ks1);
  des_set_key((const_des_cblock *) (key->data + 9), ks2);
  des_set_key((const_des_cblock *) (key->data+ 17), ks3);

  rnd_bytes(out->data, 8);
  out->data[8] = out->data[6], out->data[9] = out->data[7];
  n = 0;
  des_ede3_cfb64_encrypt(out->data, out->data, 10, ks1, ks2, ks3, &iv, &n,
			 ENCRYPT);
  iv[6] = iv[0], iv[7] = iv[1];
  memcpy(iv, out->data + 2, 6);
  n = 0;
  des_ede3_cfb64_encrypt(in->data, out->data + 10, in->length, ks1, ks2, ks3,
			 &iv, &n, ENCRYPT);
  return (0);
}

static int pgp_castencrypt(BUFFER *in, BUFFER *out, BUFFER *key)
{
  byte iv[8];
  int i, n = 0;
  CAST_KEY ks;

  assert(key->length == 17);

  for (i = 0; i < 8; i++)
    iv[i] = 0;

  CAST_set_key(&ks, 16, key->data + 1);

  rnd_bytes(out->data, 8);
  out->data[8] = out->data[6], out->data[9] = out->data[7];
  n = 0;
  CAST_cfb64_encrypt(out->data, out->data, 10, &ks, iv, &n, CAST_ENCRYPT);
  iv[6] = iv[0], iv[7] = iv[1];
  memcpy(iv, out->data + 2, 6);
  n = 0;
  CAST_cfb64_encrypt(in->data, out->data + 10, in->length, &ks, iv, &n,
		     CAST_ENCRYPT);
  return (0);
}

int pgp_symmetric(BUFFER *in, BUFFER *key)
{
  BUFFER *out;

  out = buf_new();
  buf_prepare(out, in->length + 10);
  switch (buf_getc(key)) {
#ifdef USE_IDEA
   case PGP_K_IDEA:
    pgp_ideaencrypt(in, out, key);
    break;
#endif
   case PGP_K_3DES:
    pgp_3desencrypt(in, out, key);
    break;
  case PGP_K_CAST5:
    pgp_castencrypt(in, out, key);
    break;
   default:
    errlog(ERRORMSG, "Unknown symmetric algorithm.\n");
  }
  pgp_packet(out, PGP_ENCRYPTED);

  buf_move(in, out);
  buf_free(out);
  return (0);
}

int pgp_literal(BUFFER *b, char *filename, int text)
{
  BUFFER *out;
  BUFFER *line;

  if (filename == NULL)
    filename = "stdin";

  if (strlen(filename) > 255)
    return (-1);

  out = buf_new();
  line = buf_new();

  if (text)
    buf_setc(out, 't');
  else
    buf_setc(out, 'b');
  buf_appendc(out, strlen(filename));
  buf_appends(out, filename);
  buf_appendl(out, 0);		/* timestamp */

  if (b->length > 0) {
    if (text)
      while (buf_getline(b, line) != -1) {
        buf_cat(out, line);
	buf_appends(out, "\r\n");
      } else
	buf_cat(out, b);
  }
  pgp_packet(out, PGP_LITERAL);
  buf_move(b, out);
  buf_free(out);
  buf_free(line);

  return (0);
}

int pgp_compress(BUFFER *in)
{
  int err;
  BUFFER *out;

  out = buf_new();
  buf_setc(out, 1);
  err = buf_zip(out, in, 13);
  if (err == 0) {
    pgp_packet3(out, PGP_COMPRESSED);
    buf_move(in, out);
  }
  buf_free(out);
  return (err);
}

int pgp_sessionkey(BUFFER *out, BUFFER *user, BUFFER *keyid, BUFFER *seskey,
		   char *pubring)
{
  BUFFER *encrypt, *key, *id;
  int algo, sym, err = -1;
  int i, csum = 0;
  int tempbuf = 0;

  encrypt = buf_new();
  key = buf_new();
  id = buf_new();
  if (keyid == NULL) {
    keyid = buf_new();
    tempbuf = 1;
  }
  sym = seskey->data[0];
  if ((algo = pgpdb_getkey(PK_ENCRYPT, PGP_ANY, &sym, key, user, NULL, keyid,
			   pubring, NULL)) == -1)
    goto end;

  buf_setc(out, 3);		/* type */
  buf_cat(out, keyid);
  buf_appendc(out, algo);	/* algorithm */

  buf_set(encrypt, seskey);

  for (i = 1; i < encrypt->length; i++)
    csum = (csum + encrypt->data[i]) % 65536;
  buf_appendi(encrypt, csum);

  switch (algo) {
#ifdef USE_RSA
  case PGP_ES_RSA:
    err = pgp_rsa(encrypt, key, PK_ENCRYPT);
    mpi_put(out, encrypt);
    break;
#endif
   case PGP_E_ELG:
    err = pgp_elgencrypt(encrypt, key);
    buf_cat(out, encrypt);
    break;
  default:
    errlog(NOTICE, "Unknown encryption algorithm.\n");
    err = -1;
    goto end;
  }
  if (err == -1) {
    errlog(ERRORMSG, "Encryption failed!\n");
    goto end;
  }
  pgp_packet(out, PGP_SESKEY);
end:
  if (tempbuf)
    buf_free(keyid);
  buf_free(id);
  buf_free(encrypt);
  buf_free(key);
  return (err);
}

void pgp_marker(BUFFER *out)
{
  buf_clear(out);
  buf_append(out, "PGP", 3);
  pgp_packet(out, PGP_MARKER);
}

int pgp_symsessionkey(BUFFER *out, BUFFER *seskey, BUFFER *pass)
{
  BUFFER *key;
  int sym;
  key = buf_new();

  sym = seskey->data[0];
  buf_setc(out, 4); /* version */
#ifdef MIMICPGP5
  pgp_makesk(out, key, sym, 1, PGP_H_MD5, pass);
#else
  pgp_makesk(out, key, sym, 3, PGP_H_SHA1, pass);
#endif
  if (seskey->length > 1)
    buf_cat(out, seskey);
  else {
    buf_setc(seskey, sym);
    buf_cat(seskey, key);
  }
  pgp_packet(out, PGP_SYMSESKEY);
  buf_free(key);
  return (0);
}

int pgp_digest(int hashalgo, BUFFER *in, BUFFER *d)
{
  switch (hashalgo) {
   case PGP_H_MD5:
    digest_md5(in, d);
    return (0);
   case PGP_H_SHA1:
    digest_sha1(in, d);
    return (0);
  case PGP_H_RIPEMD:
    digest_rmd160(in, d);
    return (0);
   default:
    return (-1);
  }
}    

int asnprefix(BUFFER *b, int hashalgo)
{
  switch (hashalgo) {
  case PGP_H_MD5:
    buf_append(b, MD5PREFIX, sizeof(MD5PREFIX) - 1);
    return (0);
  case PGP_H_SHA1:
    buf_append(b, SHA1PREFIX, sizeof(SHA1PREFIX) - 1);
    return (0);
  default:
    return (-1);
  }
}

int pgp_expandsk(BUFFER *key, int skalgo, int hashalgo, BUFFER *data)
{
  BUFFER *temp;
  int keylen;
  int err = 0;
  temp = buf_new();

  keylen = pgp_keylen(skalgo);
  buf_clear(key);
  while (key->length < keylen) {
    if (pgp_digest(hashalgo, data, temp) == -1) {
      err = -1;
      goto end;
    }
    buf_cat(key, temp);

    buf_setc(temp, 0);
    buf_cat(temp, data);
    buf_move(data, temp);
  }

  if (key->length > keylen) {
    buf_set(temp, key);
    buf_get(temp, key, keylen);
  }
 end:
  buf_free(temp);
  return(err);
}

int pgp_makesk(BUFFER *out, BUFFER *key, int sym, int type, int hash,
	       BUFFER *pass)
{
  int err = 0;
  BUFFER *salted;
  salted = buf_new();

  buf_appendc(out, sym);
  buf_appendc(out, type);
  buf_appendc(out, hash);
  switch (type) {
  case 0:
    buf_set(salted, pass);
    break;
  case 1:
    buf_appendrnd(salted, 8); /* salt */
    buf_cat(out, salted);
    buf_cat(salted, pass);
    break;
  case 3:
    buf_appendrnd(salted, 8); /* salt */
    buf_cat(out, salted);
    buf_appendc(out, 96); /* encoded count value 65536 */
    pgp_iteratedsk(salted, salted, pass, 96);
    break;
  default:
    err = -1;
  }
  pgp_expandsk(key, sym, hash, salted);
  buf_free(salted);
  return (err);
}

/* PGP/MIME needs to know the hash algorithm */
int pgp_signhashalgo(BUFFER *algo, BUFFER *userid, char *secring, BUFFER *pass)
{
  int pkalgo;

  pkalgo = pgpdb_getkey(PK_SIGN, PGP_ANY, NULL, NULL, userid, NULL, NULL,
			secring, pass);
  if (pkalgo == PGP_S_DSA)
    buf_sets(algo, "sha1");
  if (pkalgo == PGP_ES_RSA)
    buf_sets(algo, "md5");
  return (pkalgo > 0 ? 0 : -1);
}

int pgp_sign(BUFFER *msg, BUFFER *msg2, BUFFER *sig, BUFFER *userid,
	     BUFFER *pass, int type, int self, long now, int remail,
	     BUFFER *keypacket, char *secring)
/*  msg:      data to be signed (buffer is modified)
    msg2:     additional data to be signed for certain sig types
    sig:      signature is placed here
    userid:   select signing key
    pass:     pass phrase for signing key
    type:     PGP signature type
    self:     is this a self-signature?
    now:      time of signature creation
    remail:   is this an anonymous message?
    keypacket: signature key
    secring:   key ring with signature key */
{
  BUFFER *key, *id, *d, *sub, *enc;
  int algo, err = -1;
  int version = 3, hashalgo;
  int type1;

  id = buf_new();
  d = buf_new();
  sub = buf_new();
  enc = buf_new();
  key = buf_new();

  if (now == 0) {
    now = time(NULL);
    if (remail)
      now -= rnd_number(4 * 24 * 60 * 60);
  }
  if (keypacket) {
    buf_rewind(keypacket);
    algo = pgp_getkey(PK_SIGN, PGP_ANY, NULL, keypacket, key, id, NULL, pass);
  } else
    algo = pgpdb_getkey(PK_SIGN, PGP_ANY, NULL, key, userid, NULL, id, secring,
			pass);
  if (algo <= -1) {
    err = algo;
    goto end;
  }
  if (algo == PGP_S_DSA)
    version = 4;
  if (version == 3)
    hashalgo = PGP_H_MD5;
  else
    hashalgo = PGP_H_SHA1;

  if (!self)
    version = 3;

  switch (type) {
   case PGP_SIG_CERT:
     type1 = pgp_getpacket(msg, d);
     assert (type1 == PGP_PUBKEY);
     buf_setc(msg, 0x99);
     buf_appendi(msg, d->length);
     buf_cat(msg, d);

     pgp_getpacket(msg2, d);
     switch (version) {
     case 3:
       buf_cat(msg, d);
       break;
     case 4:
       buf_appendc(msg, 0xb4);
       buf_appendl(msg, d->length);
       buf_cat(msg, d);
       break;
     }
     break;
   case PGP_SIG_BINDSUBKEY:
     type1 = pgp_getpacket(msg, d);
     assert (type1 == PGP_PUBKEY);
     buf_clear(msg);
     buf_appendc(msg, 0x99);
     buf_appendi(msg, d->length);
     buf_cat(msg, d);
     
     type1 = pgp_getpacket(msg2, d);
     assert (type1 == PGP_PUBSUBKEY);
     buf_appendc(msg, 0x99);
     buf_appendi(msg, d->length);
     buf_cat(msg, d);
     break;
   case PGP_SIG_BINARY:
     break;
   case PGP_SIG_CANONIC:
    pgp_sigcanonic(msg);
    break;
   default:
    NOT_IMPLEMENTED;
  }
  switch (version) {
   case 3:
    buf_set(d, msg);
    buf_appendc(d, type);
    buf_appendl(d, now);
    pgp_digest(hashalgo, d, d);
    if (algo == PGP_ES_RSA)
      asnprefix(enc, hashalgo);
    buf_cat(enc, d);
    err = pgp_dosign(algo, enc, key);

    buf_setc(sig, version);
    buf_appendc(sig, 5);
    buf_appendc(sig, type);
    buf_appendl(sig, now);
    buf_cat(sig, id);
    buf_appendc(sig, algo);
    buf_appendc(sig, hashalgo);
    buf_append(sig, d->data, 2);
    buf_cat(sig, enc);
    break;

   case 4:
    buf_setc(sig, version);
    buf_appendc(sig, type);
    buf_appendc(sig, algo);
    buf_appendc(sig, hashalgo);

    buf_clear(d);
    buf_appendl(d, now);
    pgp_subpacket(d, PGP_SUB_CREATIME);
    buf_cat(sub, d);

    if (self) {
      buf_setc(d, PGP_K_3DES);
      pgp_subpacket(d, PGP_SUB_PSYMMETRIC);
      buf_cat(sub, d);
    }

    buf_appendi(sig, sub->length); /* hashed subpacket length */
    buf_cat(sig, sub);

    /* compute message digest */
    buf_set(d, msg);
    buf_cat(d, sig);
    buf_appendc(d, version);
    buf_appendc(d, 0xff);
    buf_appendl(d, sig->length);
    pgp_digest(hashalgo, d, d);

    pgp_subpacket(id, PGP_SUB_ISSUER);
    buf_appendi(sig, id->length); /* unhashed subpacket length */
    buf_cat(sig, id);

    buf_append(sig, d->data, 2);

    if (algo == PGP_ES_RSA)
      asnprefix(enc, hashalgo);
    buf_cat(enc, d);
    err = pgp_dosign(algo, enc, key);
    buf_cat(sig, enc);
    break;
  } 
  pgp_packet(sig, PGP_SIG);

end:
  buf_free(key);
  buf_free(id);
  buf_free(d);
  buf_free(sub);
  buf_free(enc);
  return (err);
}

int pgp_pubkeycert(BUFFER *userid, char *keyring, BUFFER *pass,
		   BUFFER *out, int remail)
{
  BUFFER *key;
  KEYRING *r;
  int err = -1;

  key = buf_new();
  r = pgpdb_open(keyring, pass, 0);
  if (r != NULL)
    while (pgpdb_getnext(r, key, NULL, userid) != -1) {
      if (pgp_makepubkey(key, NULL, out, pass, 0) != -1)
	err = 0;
    }
  if (err == 0)
    pgp_armor(out, remail);
  else
    buf_clear(out);
  buf_free(key);
  return (err);
}

#endif /* USE_PGP */
