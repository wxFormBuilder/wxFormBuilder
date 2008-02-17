#!/bin/sh
#

set -e

# Build premake
make -Csdk/premake/src -f../build/Makefile

# Autodetect wxWidgets settings
if wx-config --unicode >/dev/null 2>/dev/null; then
	unicode="--unicode"
fi
if ! wx-config --debug >/dev/null 2>/dev/null; then
	debug="--disable-wx-debug"
fi

sdk/premake/src/bin/premake --target cb-gcc $unicode $debug --with-wx-shared $1
echo done...
echo 
#
sdk/premake/src/bin/premake --target gnu $unicode $debug --with-wx-shared $1
echo done...
echo 
#
echo Done generating all project files for wxFormBuilder.
#
