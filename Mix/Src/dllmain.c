/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Mixmaster DLL startup
   $Id: dllmain.c,v 1.4 2002/09/18 23:26:16 rabbi Exp $ */


#include "mix3.h"
#ifdef WIN32
int WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    if(!is_nt_service()) {
      rnd_state = RND_WILLSEED;
      mix_init(NULL);
      if (rnd_state == RND_WILLSEED)
        rnd_state = RND_NOTSEEDED;
    }
    break;
  case DLL_PROCESS_DETACH:
    if(!is_nt_service())
      mix_exit();
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    return(0);
  }
  return(1);
}
#endif /* WIN32 */
