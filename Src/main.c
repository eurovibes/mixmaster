/* Mixmaster version 3  --  (C) 1999 Anonymizer Inc.

   Mixmaster may be redistributed and modified under certain conditions.
   This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
   ANY KIND, either express or implied. See the file COPYRIGHT for
   details.

   Command-line based frontend
   $Id: main.c,v 1.11 2002/08/20 19:39:24 rabbi Exp $ */


#include "mix3.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#ifdef POSIX
#include <unistd.h>
#else
#include <io.h>
#endif

static char *largopt(char *p, char *opt, char *name, int *error);
static void noarg(char *name, char p);

/** main *****************************************************************/

/* Returns:
 0 successful operation
 1 command line error
 2 client error condition */

#ifdef WIN32SERVICE
int mix_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  int error = 0, deflt = 1, help = 0, readmail = 0, send = -1, sendpool = 0,
  header = 1, maint = 0, keygen = 0, verbose = 2, sign = 0, encrypt = 0;
  int daemon = 0, type_list = 0;

#ifdef USE_SOCK
  int pop3 = 0;

#endif
  char *filename = NULL;
  int i;
  int ret = 0;
  char *p, *q;
  char chain[1024] = "";
  char nym[LINELEN] = "";
  BUFFER *nymopt, *pseudonym, *attachments;
  int numcopies = 0;		/* default value set in mix.cfg */
  BUFFER *msg, *chainlist, *field, *content;
  FILE *f;

  mix_init(NULL);

  msg = buf_new();
  chainlist = buf_new();
  nymopt = buf_new();
  pseudonym = buf_new();
  attachments = buf_new();
  field = buf_new();
  content = buf_new();

#ifdef USE_NCURSES
  if (argc == 1) {
    if (isatty(fileno(stdin)))
      menu_main();
    else
      menu_folder(0, NULL);
    goto end;
  }
#endif
  if (argc > 1 && strleft(argv[1], "-f")) {
    menu_folder(strlen(argv[1]) > 2 ? argv[1][2] : 0,
		argc < 3 ? NULL : argv[2]);
    goto end;
  }
  for (i = 1; i < argc; i++) {
    p = argv[i];
    if (p[0] == '-' && p[1] != '\0') {
      if (p[1] == '-') {
	p += 2;
	if (strieq(p, "help"))
	  help = 1, deflt = 0;
	else if (streq(p, "verbose"))
	  verbose = 1;
        else if (streq(p, "type-list"))
          type_list = 1;
	else if (streq(p, "dummy"))
	  send = MSG_NULL, deflt = 0;
	else if (streq(p, "remailer"))
	  maint = 1, deflt = 0;
	else if (streq(p, "generate-key"))
	  keygen = 2, deflt = 0;
	else if (streq(p, "update-keys"))
	  keygen = 1, deflt = 0;
	else if (streq(p, "send"))
	  sendpool = 1, deflt = 0;
	else if (streq(p, "read-mail"))
	  readmail = 1, deflt = 0;
	else if (streq(p, "store-mail"))
	  readmail = 2, deflt = 0;
#ifdef USE_SOCK
	else if (streq(p, "pop-mail"))
	  pop3 = 1, deflt = 0;
#endif
	else if (streq(p, "daemon"))
	  daemon = 1, deflt = 0;
	else if (streq(p, "post"))
	  send = MSG_POST;
	else if (streq(p, "mail"))
	  send = MSG_MAIL;
	else if (streq(p, "sign"))
	  sign = 1;
	else if (streq(p, "encrypt"))
	  encrypt = 1;
	else if ((q = largopt(p, "to", argv[0], &error)) != NULL) {
	  header = 0;
	  buf_appendf(msg, "To: %s\n", q);
	} else if ((q = largopt(p, "post-to", argv[0], &error)) != NULL) {
	  send = MSG_POST, header = 0;
	  buf_appendf(msg, "Newsgroups: %s\n", q);
	} else if ((q = largopt(p, "subject", argv[0], &error)) != NULL) {
	  buf_appendf(msg, "Subject: %s\n", q);
	} else if ((q = largopt(p, "header", argv[0], &error)) != NULL) {
	  buf_appendf(msg, "%s\n", q);
	} else if ((q = largopt(p, "chain", argv[0], &error)) != NULL) {
	  buf_appendf(msg, "Chain: %s\n", q);
	}
#ifdef USE_PGP
	else if ((q = largopt(p, "reply-chain", argv[0], &error)) != NULL) {
	  buf_appendf(msg, "Reply-Chain: %s\n", q);
	} else if ((q = largopt(p, "latency", argv[0], &error)) != NULL) {
	  buf_appendf(msg, "Latency: %s\n", q);
	} else if ((q = largopt(p, "attachment", argv[0], &error)) != NULL) {
	  buf_appendf(attachments, "%s\n", q);
	} else if ((q = largopt(p, "nym-config", argv[0], &error)) != NULL) {
	  deflt = 0;
	  strncpy(nym, q, sizeof(nym));
	  if (i > argc && strileft(argv[i + 1], "name="))
	    buf_sets(pseudonym, argv[++i] + 5);
	  else if (i > argc && strileft(argv[i + 1], "opt="))
	    buf_appends(nymopt, argv[++i] + 5);
	} else if ((q = largopt(p, "nym", argv[0], &error)) != NULL) {
	  buf_appendf(msg, "Nym: %s\n", q);
	}
#endif
	else if ((q = largopt(p, "copies", argv[0], &error)) != NULL) {
	  sscanf(q, "%d", &numcopies);
	} else if (error == 0 && mix_configline(p) == 0) {
	  fprintf(stderr, "%s: Invalid option %s\n", argv[0], argv[i]);
	  error = 1;
	}
      } else {
	while (*++p) {
	  switch (*p) {
	  case 'd':
	    send = MSG_NULL, deflt = 0;
	    break;
	  case 'R':
	    readmail = 1, deflt = 0;
	    break;
	  case 'I':
	    readmail = 2, deflt = 0;
	    break;
	  case 'S':
	    sendpool = 1, deflt = 0;
	    break;
	  case 'M':
	    maint = 1, deflt = 0;
	    break;
#ifdef USE_SOCK
	  case 'P':
	    pop3 = 1, deflt = 0;
	    break;
#endif
	  case 'D':
	    daemon = 1, deflt = 0;
	    break;
	  case 'G':
	    keygen = 2, deflt = 0;
	    break;
	  case 'K':
	    keygen = 1, deflt = 0;
	    break;
	  case 'L':		/* backwards compatibility */
	    break;
	  case 'v':
	    verbose = 1;
	    break;
	  case 'h':
	    help = 1, deflt = 0;
	    break;
	  case 'T':
	    type_list = 1;
	    break;
	  case 't':
	    if (*(p + 1) == 'o')
	      p++;
	    header = 0;
	    if (i < argc - 1)
	      buf_appendf(msg, "To: %s\n", argv[++i]);
	    else {
	      fprintf(stderr, "%s: Missing argument for option -to\n",
		      argv[0]);
	      error = 1;
	    }
	    break;
	  case 's':
	    if (i < argc - 1)
	      buf_appendf(msg, "Subject: %s\n", argv[++i]);
	    else {
	      noarg(argv[0], *p);
	      error = 1;
	    }
	    break;
	  case 'l':
	    if (i < argc - 1)
	      buf_appendf(msg, "Chain: %s\n", argv[++i]);
	    else {
	      noarg(argv[0], *p);
	      error = 1;
	    }
	    break;
	  case 'r':
	    if (i < argc - 1)
	      buf_appendf(msg, "Reply-Chain: %s\n", argv[++i]);
	    else {
	      noarg(argv[0], *p);
	      error = 1;
	    }
	    break;
#ifdef USE_PGP
	  case 'n':
	    if (i < argc - 1)
	      buf_appendf(msg, "Nym: %s\n", argv[++i]);
	    else {
	      noarg(argv[0], *p);
	      error = 1;
	    }
	    break;
#endif
	  case 'c':
	    if (i < argc - 1)
	      sscanf(argv[++i], "%d", &numcopies);
	    else {
	      noarg(argv[0], *p);
	      error = 1;
	    }
	    break;
	  case 'p':
	    send = MSG_POST;
	    break;
	  case 'g':
	    if (i < argc - 1) {
	      send = MSG_POST, header = 0;
	      buf_appendf(msg, "Newsgroups: %s\n", argv[++i]);
	    } else {
	      noarg(argv[0], *p);
	      error = 1;
	    }
	    break;
	  case 'a':
	    if (i < argc - 1)
	      buf_appendf(attachments, "%s\n", argv[++i]);
	    else {
	      noarg(argv[0], *p);
	      error = 1;
	    }
	    break;
	  case 'm':
	    send = MSG_MAIL;
	    break;
	  default:
	    fprintf(stderr, "%s: Invalid option -%c\n", argv[0], *p);
	    error = 1;
	    break;
	  }
	}
      }
    } else {
      if (strchr(argv[i], '@')) {
	header = 0;
	buf_appendf(msg, "To: %s\n", argv[i]);
      } else {
	if (filename == NULL)
	  filename = argv[i];
	else {
	  fprintf(stderr, "%s: Error in command line: %s\n", argv[0], argv[i]);
	  error = 1;
	}
      }
    }
  }

  if (error) {
    ret = 1;
    goto end;
  }
  if (type_list) {
    if (print_type2list() < 0) {
      fprintf(stderr, "Cannot print type2.list.\n");
      ret = 2;
    }
    goto end;
  }
  if (help || (isatty(fileno(stdin)) && isatty(fileno(stdout))))
    fprintf(stderr, "Mixmaster %s - %s\n", VERSION, COPYRIGHT);

  if (help) {
    printf("Usage: %s [options] [user@host] [filename]\n\n", argv[0]);
    printf("Options:\n\
\n\
-h, --help                        summary of command line options\n\
-T, --type-list                   list available remailers\n\
-t, --to=user@host                the recipient's address(es)\n\
-g, --post-to=newsgroup           newsgroup(s) to post to\n\
-p, --post                        input is a Usenet article\n\
-m, --mail                        input is a mail message\n\
-s, --subject=subject             message subject\n\
    --header='header line'        arbitrary message headers\n\
-a, --attachment=file             attach a file\n"
#ifdef USE_PGP
	   "-n, --nym=yournym                 use pseudonym to send the message\n\
    --encrypt                     encrypt the message using the PGP format\n\
    --sign                        sign the message using the PGP format\n"
#endif
	   "-l, --chain=mix1,mix2,mix3,...    specify a remailer chain\n\
-c, --copies=num                  send num copies to increase reliability\n\
-d, --dummy                       generate a dummy message\n\
-S, --send                        send the message(s) in the pool\n"
#ifdef USE_PGP
	   "    --nym-config=yournym          generate a new pseudonym\n\
    --latency=hours               reply chain latency\n\
    --reply-chain=rem1,rem2,...   reply chain for the pseudonym\n"
#endif
	   "-v, --verbose                     output informational messages\n\
-f [file]                         read a mail folder\n"
#ifndef USE_NCURSES
	   "\n-fr, -ff, -fg [file]              send reply/followup/group reply to a message\n"
#endif
	   "\nThe input file is expected to contain mail headers if no address is\n\
specified in the command line.\n\
\n\
Remailer:\n\
\n\
-R, --read-mail                   read remailer message from stdin\n\
-I, --store-mail                  read remailer msg from stdin, do not decrypt\n\
-M, --remailer                    process the remailer pool\n\
-D, --daemon                      remailer as background process\n"
#ifdef USE_SOCK
	   "-S, --send                        force sending messages from the pool\n"
#endif
	   "-P, --pop-mail                    force getting messages from POP3 servers\n\
-G, --generate-key                generate a new remailer key\n\
-K, --update-keys                 generate remailer keys if necessary\n"
#ifdef WIN32SERVICE
	   "\n\
WinNT service:\n\
\n\
    --install-svc                 install the service\n\
    --remove-svc                  remove the service\n\
    --run-svc                     run as a service\n"
#endif
    );

    ret = 0;
    goto end;
  }
  if (deflt && send == -1)
    send = MSG_MAIL;
  if (nym[0] != 0)
    send = -1;
  if ((send == MSG_MAIL || send == MSG_POST) && filename == NULL &&
      header == 1 && isatty(fileno(stdin))) {
    /* we don't get here if USE_NCURSES is set */
    printf("Run `%s -h' to view a summary of the command line options.\n\nEnter the message, complete with headers.\n",
	   argv[0]);
#ifdef UNIX
    printf("When done, press ^D.\n\n");
#else
    printf("When done, press ^Z.\n\n");
#endif
  }
  if (header == 0)
    buf_nl(msg);

  if (readmail || send == MSG_MAIL || send == MSG_POST) {
    if (filename == NULL || streq(filename, "-"))
      f = stdin;
    else {
      f = fopen(filename, "r");
      if (f == NULL)
	fprintf(stderr, "Can't open %s.\n", filename);
    }

    if (f && buf_read(msg, f) != -1) {
      if (readmail == 1)
	mix_decrypt(msg);
      else if (readmail == 2)
	pool_add(msg, "inf");
      if (send == MSG_MAIL || send == MSG_POST) {
	BUFFER *sendmsg;
	int numdest = 0;

	sendmsg = buf_new();

	while (buf_getheader(msg, field, content) == 0) {
	  if (bufieq(field, "nym")) {
	    strncpy(nym, content->data, sizeof(nym));
	  } else if (bufieq(field, "chain"))
	    if (strchr(content->data, ';')) {
	      i = strchr(content->data, ';') - (char *)content->data;
	      strncpy(chain, content->data, i);
	      if (strstr(content->data + i, "copies=") != NULL) {
		sscanf(strstr(content->data + i, "copies=") +
		       sizeof("copies=") - 1, "%d", &numcopies);
	      }
	    } else
	      strncpy(chain, content->data, sizeof(chain));
	  else {		/* line goes into message */
	    if ((send == MSG_MAIL && bufieq(field, "to"))
		|| (send == MSG_POST && bufieq(field, "newsgroups")))
	      numdest++;
	    if (bufieq(field, "from"))
	      fprintf(stderr, "Warning: The message has a From: line.\n");
	    buf_appendheader(sendmsg, field, content);
	  }
	}
	buf_nl(sendmsg);
	buf_rest(sendmsg, msg);

	while (buf_getline(attachments, field) != -1)
	  if (attachfile(sendmsg, field) == -1) {
	    errlog(ERRORMSG, "Can't attach %b!\n", field);
	    ret = 2;
	    goto end;
	  }

#ifdef USE_PGP
	if (nym[0] != 0 && strchr(nym, '@') == NULL)
	  strcatn(nym, "@", sizeof(nym));
	if (sign || encrypt) {
	  BUFFER *pass;

	  pass = buf_new();
	  user_pass(pass);
	  if (pgp_mailenc((encrypt ? PGP_ENCRYPT : 0) |
			  (nym[0] != 0 && sign ? PGP_SIGN : 0) |
			  PGP_TEXT | PGP_REMAIL, sendmsg, nym,
			  pass, NULL, NYMSECRING) != 0) {
	    fprintf(stderr, "Encryption failed: missing key!");
	    ret = 2;
	    goto end;
	  }
	  buf_free(pass);
	}
	if (nym[0] != 0) {
	  if (nym_encrypt(sendmsg, nym, send) == 0)
	    send = MSG_MAIL;
	  else
	    fprintf(stderr, "Nym error, sending message anonymously.\n");
	}
#endif
	if (numdest == 0) {
	  fprintf(stderr, "No destination address given!\n");
	  ret = 2;
	} else if (numcopies < 0 || numcopies > 10) {
	  fprintf(stderr, "Invalid number of copies!\n");
	  ret = 2;
	} else {
	  if (mix_encrypt(send, sendmsg, chain, numcopies,
			  chainlist) == -1) {
	    ret = 2;
	    if (chainlist->length)
	      fprintf(stderr, "%s\n", chainlist->data);
	    else
	      fprintf(stderr, "Failed!\n");
	  } else if (verbose) {
	    fprintf(stderr, "Chain: ");
	    buf_write(chainlist, stderr);
	  }
	}

	buf_free(sendmsg);
      }
      if (filename != NULL)
	fclose(f);
    } else
      ret = 2;
  }
  if (send == MSG_NULL) {
    if (msg->length) {
      while (buf_getheader(msg, field, content) == 0) {
	if (bufieq(field, "chain"))
	  strncpy(chain, content->data, sizeof(chain));
      }
    }
    if (mix_encrypt(MSG_NULL, NULL, chain, numcopies, chainlist) == -1) {
      ret = 2;
      if (chainlist->length)
	fprintf(stderr, "%s\n", chainlist->data);
      else
	fprintf(stderr, "Failed!\n");
    } else if (verbose) {
      fprintf(stderr, "Chain: ");
      buf_write(chainlist, stderr);
    }
  }
#ifdef USE_PGP
  if (nym[0] != 0) {
    char nymserver[LINELEN] = "*";
    BUFFER *chains;

    chains = buf_new();
    if (numcopies < 1 || numcopies > 10)
      numcopies = 1;
    while (buf_getheader(msg, field, content) != -1) {
      if (bufieq(field, "chain"))
	strncpy(chain, content->data, sizeof(chain));
      else if (bufieq(field, "reply-chain"))
	buf_appendf(chains, "Chain: %b\n", content);
      else if (field->length)
	buf_appendheader(chains, field, content);
      else
	buf_nl(chains);
    }
    if (strrchr(nym, '@')) {
      strncpy(nymserver, strrchr(nym, '@'), sizeof(nymserver));
      *strrchr(nym, '@') = '\0';
    }
    if (nym_config(NYM_CREATE, nym, nymserver, pseudonym,
		   chain, numcopies, chains, nymopt) < 0) {
      ret = 2;
      fprintf(stderr, "Failed!\n");
    }
    user_delpass();
    buf_free(chains);
  }
#endif

  if (keygen)
    keymgt(keygen);
  if (sendpool)
    mix_send();
#ifdef USE_SOCK
  if (pop3)
    pop3get();
#endif
  if (maint)
    mix_regular(0);

end:
  buf_free(field);
  buf_free(content);
  buf_free(chainlist);
  buf_free(msg);
  buf_free(nymopt);
  buf_free(pseudonym);
  buf_free(attachments);

  if (daemon) {
#ifdef UNIX
    int pid;

    fprintf(stderr, "Detaching.\n");
    /* Detach as suggested by the Unix Programming FAQ */
    pid = fork();
    if (pid > 0)
      exit(0);
    if (setsid() < 0) {
      /* This should never happen. */
      fprintf(stderr, "setsid() failed.\n");
      exit(1);
    };
    pid = fork();
    if (pid > 0)
      exit(0);
    if (chdir(MIXDIR) < 0) {
      if (chdir("/") < 0) {
        fprintf(stderr, "Cannot chdir to mixdir or /.\n");
        exit(1);
      };
    };
    freopen ("/dev/null", "r", stdin);
    freopen ("/dev/null", "w", stdout);
    freopen ("/dev/null", "w", stderr);
#endif
    mix_daemon();
  }
  mix_exit();
  return (ret);
}

static char *largopt(char *p, char *opt, char *name, int *error)
{
  if (streq(p, opt)) {
    fprintf(stderr, "%s: Missing argument for option --%s\n", name, p);
    *error = 1;
  } else if (strleft(p, opt) && p[strlen(opt)] == '=') {
    return (p + strlen(opt) + 1);
  }
  return (NULL);
}

static void noarg(char *name, char p)
{
  fprintf(stderr, "%s: Missing argument for option -%c\n", name, p);
}
