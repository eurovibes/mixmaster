/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Interface to cryptographic library
   $Id: crypto.h,v 1.2 2002/07/22 17:54:48 rabbi Exp $ */


#ifndef _CRYPTO_H
#define _CRYPTO_H
#include "mix3.h"

#ifdef USE_OPENSSL
#include <openssl/opensslv.h>
#if (OPENSSL_VERSION_NUMBER < 0x0903100)
#error "This version of OpenSSL is not supported. Please get a more current verion from http://www.openssl.org"
#endif
#include <openssl/des.h>
#include <openssl/blowfish.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <openssl/bn.h>
#include <openssl/dh.h>
#include <openssl/dsa.h>
#ifdef USE_RSA
#include <openssl/rsa.h>
#endif
#ifdef USE_IDEA
#include <openssl/idea.h>
#endif
#ifdef USE_AES
#include <openssl/aes.h>
#endif
#include <openssl/cast.h>
#include <openssl/rand.h>

#ifdef USE_RSA
typedef RSA PUBKEY;
typedef RSA SECKEY;
#endif

#else
/* #error "No crypto library." */
typedef void PUBKEY;
typedef void SECKEY;
#endif

#endif
