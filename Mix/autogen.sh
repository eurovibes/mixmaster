#! /bin/sh

set -e

#aclocal-1.7 || aclocal-1.6 || aclocal
#autoheader
#automake-1.7 --gnu -a || automake-1.6 --gnu -a || automake --gnu -a
autoconf
