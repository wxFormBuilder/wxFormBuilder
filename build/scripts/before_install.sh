#!/bin/sh
#
# This script is used by Travis CI to install the dependencies before building
# wxFormBuilder but can also be run by hand if necessary but currently it only
# works for Ubuntu 12.04 and 14.04 and OS X used by Travis builds.

SUDO=sudo

case $(uname -s) in
    Linux)
        if [ "$CXX" = "g++" ]; then $SUDO add-apt-repository -y ppa:ubuntu-toolchain-r/test; fi
        $SUDO apt-get update -qq
        $SUDO apt-get install -y libwxgtk3.0-dev libwxgtk-media3.0-dev cmake
        if [ "$CXX" = "g++" ]; then $SUDO apt-get install -qq g++-4.9; fi
        ;;

    Darwin)
        brew update
        brew install wxwidgets
        ;;
esac