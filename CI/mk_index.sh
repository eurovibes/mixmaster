#! /bin/bash

CLANG_RESULTS="clang/$(ls -1 report/clang)/index.html"
GCC_RESULTS="gcc/$(ls -1 report/gcc)/index.html"

sed -e "s!@STATIC_CLANG@!$(CLANG_RESULTS)!" \
    -e "s!@STATIC_GCC@!$(GCC_RESULTS)!" \
    CI/index.template > report/index.html
