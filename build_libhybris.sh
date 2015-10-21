#!/bin/bash
set -x

# settings
MACHYBRIS_HOME=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
ln -s libhybris.vpu.* libhybris.vpu
ln -s libhybris.github.* libhybris.github
LIBHYBRIS1=$MACHYBRIS_HOME/libhybris.vpu
ANDROIDHEADERS1=$MACHYBRIS_HOME/android-headers.ubuntu
LIBHYBRIS2=$MACHYBRIS_HOME/libhybris.github
ANDROIDHEADERS2=$MACHYBRIS_HOME/android-headers.github

# get build dependencies
sudo apt-get install -y --force-yes build-essential
sudo apt-get install -y --force-yes git autoconf libtool pkg-config

# build patched ubuntu's libhybris
cd $MACHYBRIS_HOME/libhybris.vpu.*
cd hybris
./autogen.sh --enable-debug --enable-trace  --prefix=/usr/local --with-android-headers=$ANDROIDHEADERS1
make clean
make -j8

# build patched github's libhybris
cd $MACHYBRIS_HOME/libhybris.github.*
cd hybris
./autogen.sh --enable-debug --enable-trace  --prefix=/usr/local --with-android-headers=$ANDROIDHEADERS2
make clean
make -j8

# install libhybris: first ubuntu+vpu one, and then githubs one on top of that
cd $MACHYBRIS_HOME/libhybris.vpu.*
cd hybris
#sudo make install
cd $MACHYBRIS_HOME/libhybris.github.*
cd hybris
#sudo make install

cd $MACHYBRIS_HOME
exit
