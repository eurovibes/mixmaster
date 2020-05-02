#! /bin/sh

echo "placeholder for git changelog" > ChangeLog
mkdir -p m4 && \
    autoreconf -sif
