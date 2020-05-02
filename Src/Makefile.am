bin_PROGRAMS = mixmaster # mpgp remailer

mixmaster_SOURCES = rndseed.c menu.c menu.h menusend.c menunym.c \
	menuutil.c menustats.c main.c
mixmaster_CFLAGS = -O3 -g -Wall -Wextra
mixmaster_LDADD = libmix.la @NCURSES_LIBS@

mpgp_SOURCES = rndseed.c dummy.c mpgp.c
mpgp_CFLAGS = -O3 -g -Wall -Wextra
mpgp_LDADD = libmix.la

remailer_SOURCES = rndseed.c dummy.c remailer.c
remailer_CFLAGS = -O3 -g -Wall -Wextra
remailer_LDADD = libmix.la

lib_LTLIBRARIES = libmix.la
libmix_la_SOURCES = mix.c rem.c rem1.c rem2.c chain.c chain1.c chain2.c \
	nym.c pgp.c pgp.h pgpdb.c pgpdata.c pgpget.c pgpcreat.c pool.c \
	mail.c rfc822.c mime.c keymgt.c compress.c stats.c crypto.c crypto.h \
	random.c util.c buffers.c maildir.c parsedate.y mix3.h mix.h intern.h
libmix_la_CFLAGS = -O3 -g -Wall -Wextra @IDEA_CFLAGS@ @NCURSES_CFLAGS@ \
	@PCRE_CFLAGS@ @SSL_CFLAGS@ @ZLIB_CFLAGS@
libmix_la_LIBADD = @IDEA_LIBS@ @PCRE_LIBS@ @SSL_LIBS@ @ZLIB_LIBS@

# 1) If the library source code has changed at all since the last update, then
#    increment revision ("c:r:a" becomes "c:r+1:a").
# 2) If any interfaces have been added, removed, or changed since the last
#    update, increment current, and set revision to 0.
# 3) If any interfaces have been added since the last public release,
#    then increment age.
# 4) If any interfaces have been removed or changed since the last public
#    release, then set age to 0.

libmix_la_LDFLAGS = -version-info 1:0:0

MAINTAINERCLEANFILES = Makefile.in
