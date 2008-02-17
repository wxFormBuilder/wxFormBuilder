#!/bin/sh
#

set -e

# Build premake
PREMAKE_DIR=sdk/premake
cp -f $PREMAKE_DIR/build/Makefile $PREMAKE_DIR/src/Makefile
cp -f $PREMAKE_DIR/build/Src-Makefile $PREMAKE_DIR/src/Src/Makefile
make -C$PREMAKE_DIR/src

# Autodetect wxWidgets settings
if wx-config --unicode >/dev/null 2>/dev/null; then
	unicode="--unicode"
fi
if ! wx-config --debug >/dev/null 2>/dev/null; then
	debug="--disable-wx-debug"
fi

$PREMAKE_DIR/src/bin/premake --target cb-gcc $unicode $debug --with-wx-shared $1
echo done...
echo 
#
$PREMAKE_DIR/src/bin/premake --target gnu $unicode $debug --with-wx-shared $1
echo done...
echo 
#
echo Done generating all project files for wxFormBuilder.
#
