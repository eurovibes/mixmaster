SUBDIRS = Src

EXTRA_DIST = tools/ChangeLog-from-git mixmaster.1 mpgp.1

man_MANS = mixmaster.1 mpgp.1

DISTCLEANFILES = *~

MAINTAINERCLEANFILES = ChangeLog INSTALL Makefile.in aclocal.m4 compile \
	config.guess config.h.in config.rpath config.sub configure depcomp \
	install-sh ltmain.sh m4/* missing ylwrap

ACLOCAL_AMFLAGS = -I m4

dist-hook:
	$(top_srcdir)/tools/ChangeLog-from-git $(top_srcdir)
