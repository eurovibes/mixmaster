#! /bin/bash

sed -e "s,%MIXDIR,," \
    -e "s,%LIBS,," \
    -e "s!%LDFLAGS!-lz -lcrypto -lncurses -lpcre!" \
    -e "s,%INC,," \
    -e "s,%DEF,-DUSE_ZLIB -DUSE_PCRE -DUSE_NCURSES -DHAVE_NCURSES_H -DGLOBALMIXCONF='\"/etc/mixmaster/client.conf\"' -DHOMEMIXDIR='\".Mix\"'," \
    < Src/Makefile.in > Src/Makefile

sed -e "s,-DNDEBUG,-DDEBUG," < Src/Makefile > Src/Makefile.debug
