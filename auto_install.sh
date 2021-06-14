#!/bin/sh

#    Copyright (c) 2020-2021 contributors as noted in the AUTHORS file
#
#   This file is part of dorm.

# Script to generate all required files from fresh git checkout.


command -v libtoolize >/dev/null 2>&1
if  [ $? -ne 0 ]; then
    command -v libtool >/dev/null 2>&1
    if  [ $? -ne 0 ]; then
        echo "autogen.sh: error: could not find libtool.  libtool is required to run autogen.sh." 1>&2
        exit 1
    fi
fi

command -v autoreconf >/dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "autogen.sh: error: could not find autoreconf.  autoconf and automake are required to run autogen.sh." 1>&2
    exit 1
fi

mkdir -p ./config
if [ $? -ne 0 ]; then
    echo "autogen.sh: error: could not create directory: ./config." 1>&2
    exit 1
fi

autoreconf --install --force --verbose -I config
res=$?
if [ "$res" -ne 0 ]; then
    echo "autogen.sh: error: autoreconf exited with status $res" 1>&2
    exit 1
fi

./configure

sudo make install
