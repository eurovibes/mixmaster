#! /bin/sh

set -e

aclocal-1.7 || aclocal-1.6 || aclocal
autoheader
automake-1.7 --add-missing --copy || automake-1.6 --add-missing --copy || automake --add-missing --copy
autoconf
