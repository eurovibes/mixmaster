/* Mixmaster version 2.9  --  (C) 1999 - 2003 Anonymizer Inc. and others.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Prepare messages for remailer chain
   $Id$ */


#include "mix3.h"
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>

void clienterr(BUFFER *msgbuf, char *err)
{
  if (msgbuf) {
    buf_sets(msgbuf, "Error: ");
    buf_appends(msgbuf, err);
  } else
    errlog(ERRORMSG, "%s\n", err);
}

int chain_select(int hop[], char *chainstr, int maxrem, REMAILER *remailer,
		 int type, BUFFER *feedback)
{
  int len = 0;
  int i, j, k;
  BUFFER *chain, *selected, *addr;
  chain = buf_new();
  selected = buf_new();
  addr = buf_new();

  if (chainstr == NULL || chainstr[0] == '\0')
    buf_sets(chain, CHAIN);
  else
    buf_sets(chain, chainstr);

  /* put the chain backwards: final hop is in hop[0] */

  for (i = chain->length; i >= 0; i--)
    if (i == 0 || chain->data[i - 1] <= ' ' || chain->data[i - 1] == ','
	|| chain->data[i - 1] == ';' || chain->data[i - 1] == ':') {
      for (j = i; isspace(chain->data[j]);)	/* ignore whitespace */
	j++;
      if (chain->data[j] == '\0')
	break;

      if (chain->data[j] == '*')
	k = 0;
#if 0
      else if (isdigit(chain->data[j]))
	k = atoi(chain->data + j);
#endif /* 0 */
      else {
	buf_sets(selected, chain->data + j);
	rfc822_addr(selected, addr);
	buf_clear(selected);
	buf_getline(addr, selected);
	if (!selected->length)
	  buf_sets(selected, chain->data + j);

	for (k = 0; k < maxrem; k++)
	  if (((remailer[k].flags.mix && type == 0) ||
	       (remailer[k].flags.cpunk && type == 1) ||
	       (remailer[k].flags.newnym && type == 2)) &&
	      (streq(remailer[k].name, selected->data) ||
	       strieq(remailer[k].addr, selected->data) ||
	       (selected->data[0] == '@' && strifind(remailer[k].addr,
					    selected->data))))
	    break;
      }
      if (k < 0 || k >= maxrem) {
	if (feedback != NULL) {
		buf_appendf(feedback, "No such remailer: %b", selected);
		buf_nl(feedback);
	}
#if 0
	k = 0;
#else /* end of 0 */
	len = -1;
	goto end;
#endif /* else not 0 */
      }
      hop[len++] = k;
      if (len >= 20) {          /* array passed in is has length 20 */
	if (feedback != NULL) {
		buf_appends(feedback, "Chain too long.\n");
	}
	break;
      }
      if (i > 0)
	chain->data[i - 1] = '\0';
    }
end:
  buf_free(chain);
  buf_free(selected);
  buf_free(addr);
  return len;
}

int chain_randfinal(int type, REMAILER *remailer, int maxrem, int rtype)
{
  int num = 0;
  int i;
  int t;

  t = rtype;
  if (rtype == 2)
    t = 1;

  /* select a random final hop */
  for (i = 1; i < maxrem; i++) {
    if (((remailer[i].flags.mix && rtype == 0) ||
	 (remailer[i].flags.pgp && remailer[i].flags.ek && rtype == 1) ||
	 (remailer[i].flags.newnym && rtype == 2)) &&
	remailer[i].info[t].reliability >= 100 * RELFINAL &&
	remailer[i].info[t].latency <= MAXLAT &&
	(type == MSG_NULL || !remailer[i].flags.middle) &&
	(remailer[i].flags.post || type != MSG_POST))
      num++;
  }
  if (num == 0)
    i = -1;
  else {
    do
      i = rnd_number(maxrem) + 1;
    while (!(((remailer[i].flags.mix && rtype == 0) ||
	      (remailer[i].flags.pgp && remailer[i].flags.ek && rtype == 1) ||
	      (remailer[i].flags.newnym && rtype == 2)) &&
	     remailer[i].info[t].reliability >= 100 * RELFINAL &&
	     remailer[i].info[t].latency <= MAXLAT &&
	     (type == MSG_NULL || !remailer[i].flags.middle) &&
	     (remailer[i].flags.post || type != MSG_POST)));
  }
  return (i);
}

int chain_rand(REMAILER *remailer, int maxrem,
	       int thischain[], int chainlen, int t)
     /* set random chain. returns 0 if not random, 1 if random, -1 on error */
{
  int hop;
  int err = 0;

  assert(t == 0 || t == 1);

  for (hop = 0; hop < chainlen; hop++)
    if (thischain[hop] == 0) {
      int select[MAXREM];
      int randavail = 0;
      int i;

      err = 1;
      for (i = 1; i < maxrem; i++)
	select[i] = ((remailer[i].flags.mix && t == 0) ||
		     (remailer[i].flags.pgp && remailer[i].flags.ek && t == 1))
	  && remailer[i].info[t].reliability >= 100 * MINREL &&
	  remailer[i].info[t].latency <= MAXLAT,
	  randavail += select[i];

      for (i = hop - DISTANCE; i <= hop + DISTANCE; i++)
	if (i >= 0 && i < chainlen && select[thischain[i]])
	  select[thischain[i]] = 0, randavail--;

      if (randavail < 1) {
	err = -1;
	goto end;
      }
      do
	thischain[hop] = rnd_number(maxrem - 1) + 1;
      while (!select[thischain[hop]]);
    }
end:
  return (err);
}

int mix_encrypt(int type, BUFFER *message, char *chainstr, int numcopies,
		BUFFER *chainlist)
{
  return (mix2_encrypt(type, message, chainstr, numcopies, chainlist));
}

/* float chain_reliablity(char *chain, int chaintype,
	                  char *reliability_string);
 *
 * Compute reliablity of a chain.
 *
 * We get the reliablity of the chain by multiplying the reliablity of
 * every remailer in the chain. The return value is the reliablity of
 * the chain, or a negative number if the reliablity can not be
 * calculated. There are two reasons why may not be able to calculated
 * the reliablity: A remailer in the chain is selected randomly, or we
 * don't have statistics about one of the remailers in the chain.
 * remailer_type indicates the remailer type:
 * 0 = Mixmaster, 1 = Cypherpunk
 *
 * If reliability_string is non-NULL, the reliability is also returned
 * as a string in this variable. The size of the string must be at
 * least 9 characters!
 *
 * This function has been added by Gerd Beuster. (gb@uni-koblenz.de)
 *--------------------------------------------------------------------*/

float chain_reliability(char *chain, int chaintype,
			char *reliability_string){

  float acc_reliability = 1; /* Accumulated reliablity */
  char *name_start, *name_end; /* temporary pointers used
				 in string scanning */
  char remailer_name[20]; /* The length of the array is taken from mix3.h. */
  int error = 0;
  int maxrem;
  int i;
  REMAILER remailer[MAXREM];

  /* chaintype 0=mix 1=ek 2=newnym */
  assert((chaintype == 0) || (chaintype == 1));
  maxrem = (chaintype == 0 ? mix2_rlist(remailer) : t1_rlist(remailer));

  /* Dissect chain */
  name_start = chain;
  name_end = chain;
  while(*name_end != '\0'){ /* While string not scanned completely */
    do /* Get next remailer */
      name_end+=sizeof(char);
    while( (*name_end != ',') && (*name_end != '\0'));
    strncpy(remailer_name, name_start,
	    (name_end - name_start) / sizeof(char) + 1*sizeof(char));
    remailer_name[name_end-name_start]='\0';
    /* Lookup reliablity for remailer remailer_name */
    for(i=0;
	(i < maxrem) && (strcmp(remailer[i].name, remailer_name) != 0);
	i++);
    if(!strcmp(remailer[i].name, remailer_name)) /* Found it! */
      acc_reliability *=
	((float) remailer[i].info[chaintype].reliability) / 10000;
    else
      error = 1; /* Did not find this remailer. We can't calculate
		    the reliablity for the whole chain. */
    name_start = name_end+sizeof(char);
  }

  if(error || (name_start==name_end))
    acc_reliability = -1;

  /* Convert reliability into string, if appropriate */
  if(reliability_string){
    if(acc_reliability < 0)
      sprintf(reliability_string, "  n/a  ");
    else{
      sprintf(reliability_string, "%6.2f", acc_reliability*100);
      *(reliability_string+6*sizeof(char)) = '%';
    }
  }

  return acc_reliability;
}

