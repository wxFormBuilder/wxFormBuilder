#!/bin/sh

# Parse command line options
wxroot=""
wxpath=`wx-config --prefix`

# These works only on wxWidgets 2.8.10+
#wxcharset=`wx-config --query-chartype`
#wxversion=`wx-config --query-version`
for args in "$@"
do
    haveroot=`expr "${args}" : '--wx-root=.*'`
    if ( [ ${args} = "--help" ] || [ ${args} = "-h" ] ); then
        echo "Available options:"
        echo
        echo "--disable-mediactrl       Disable wxMediaCtrl / wxMedia library."
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
#       echo
#       echo " --wx-version             Specify the wxWidgets version."
#       echo "                          Example: --wx-version=2.9"
#       echo "                          Current: $wxver"
        echo
        exit
    elif [ ${args} = "--disable-mediactrl" ]; then
        mediactrl="--disable-mediactrl"
        continue
    elif [ ${args} = "--disable-unicode" ]; then
        wxunicode="--disable-unicode"
        continue
    elif ( [ "$haveroot" -gt "0" ] ); then
        wxroot=${args}
        continue
    fi
done

# Autodetect wxWidgets version
if [ "$wxroot" = "" ]; then
    wxver=`wx-config --version`
else
    wxpath=${wxroot#-*=}
    wxver=`$wxpath/wx-config --version`
fi

wxversion="--wx-version="`expr substr $wxver 1 3`

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

#./premake/$platform/bin/release/premake4 --file=./premake/solution.lua $wxunicode $wxroot $wxversion $mediactrl codeblocks
./premake/$platform/bin/release/premake4 --file=./premake/solution.lua $wxunicode $wxroot $wxversion $mediactrl codelite
./premake/$platform/bin/release/premake4 --file=./premake/solution.lua $wxunicode $wxroot $wxversion $mediactrl gmake

#if [ "$platform" = "macosx" ]; then
#   ./premake4/macosx/bin/release/premake4 --file=./premake4/solution.lua xcode3
#fi

