/* Mixmaster version 2.9  --  (C) 1999 - 2002 Anonymizer Inc. and others.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Process Mixmaster 3 remailer messages
   $Id$ */


#include "mix3.h"

int v3_magic(byte *b)
{
  return (b[0] == MAGIC0 && b[1] == MAGIC1);
}

int mix3_decrypt(BUFFER *m)
{
  NOT_IMPLEMENTED;
}
