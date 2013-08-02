#!/bin/sh

# Parse command line options
shared=""
arch=""
wxroot=""
wxpath=`wx-config --prefix`

# These works only on wxWidgets 2.8.10+
#wxcharset=`wx-config --query-chartype`
#wxversion=`wx-config --query-version`
for args in "$@"
do
    haveroot=`expr "${args}" : '--wx-root=.*'`
	havearch=`expr "${args}" : '--architecture=.*'`
	haverpath=`expr "${args}" : '--rpath=.*'`
    if ( [ ${args} = "--help" ] || [ ${args} = "-h" ] ); then
			echo "Available options:"
			echo
			echo "--disable-mediactrl       Disable wxMediaCtrl / wxMedia library."
			echo
			echo "--disable-shared          Use static wxWidgets build instead of shared libraries."
			echo
			echo "--disable-unicode         Whether to use an Unicode or an ANSI build."
			echo "                          Ignored in wxWidgets 2.9 and later."
			echo "                          Example: --disable-unicode produces an ANSI build."
			echo "                          Default: Unicode build on all versions."
			#       echo "                          Current: $wxcharset"
			echo
			echo "--wx-root                 Specify the wxWidgets build path,"
			echo "                          useful for wxWidgets builds not installed"
			echo "                          in your system (alternate/custom builds)"
			echo "                          Example: --wx-root=/home/devel/wx/3.0/buildgtk"
			echo "                          Current: $wxpath"
			echo
			echo "--architecture            Specify build architecture (e.g. --architecture=i386)."
			echo "--rpath                   Specify a rpath  (e.g. --rpath=/usr/lib/wxformbuilder)."
			echo
			exit
    elif [ ${args} = "--disable-mediactrl" ]; then
        mediactrl="--disable-mediactrl"
        continue
    elif [ ${args} = "--disable-unicode" ]; then
        wxunicode="--disable-unicode"
        continue
	elif [ ${args} = "--disable-shared" ]; then
        shared="--disable-shared"
        continue
	elif [ ${args} = "--disable-unicode" ]; then
        wxunicode="--disable-unicode"
        continue
    elif ( [ "$haveroot" -gt "0" ] ); then
        wxroot=${args}
        continue
	elif ( [ "$havearch" -gt "0" ] ); then
        arch=${args}
        continue
    elif ( [ "$haverpath" -gt "0" ] ); then
        rpath=${args}
        continue
    fi
done

# Autodetect wxWidgets version
if [ "$wxroot" = "" ]; then
    wxver=`wx-config --release`
else
    wxpath=${wxroot#-*=}
    wxver=`$wxpath/wx-config --release`
fi

wxversion="--wx-version="$wxver

# Autodetect OS
isbsd=`expr "$unamestr" : '.*BSD'`
platform="unknown"
unamestr=$(uname)

if ( [ "$isbsd" -gt "0" ] ); then
    platform="bsd"
elif [ "$unamestr" = "Linux" ]; then
    platform="linux"
elif [ "$unamestr" = "Darwin" ]; then
    platform="macosx"
fi

# Build premake
cd build
make CONFIG=Release -C./premake/$platform

./premake/$platform/bin/release/premake4 --file=./premake/solution.lua $wxunicode $wxroot $wxversion $mediactrl $shared $arch codeblocks
./premake/$platform/bin/release/premake4 --file=./premake/solution.lua $wxunicode $wxroot $wxversion $mediactrl $shared $arch $rpath codelite
./premake/$platform/bin/release/premake4 --file=./premake/solution.lua $wxunicode $wxroot $wxversion $mediactrl $shared $arch $rpath gmake
if [ "$platform" = "macosx" ]; then
   ./premake/$platform/bin/release/premake4 --file=./premake/solution.lua $wxunicode $wxroot $wxversion $mediactrl $shared $arch xcode3
fi

