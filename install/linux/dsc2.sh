#!/bin/bash
#*****************************************************************************
#* Copyright (C) 2007 Ryan Mulder
#*
#* This program is free software; you can redistribute it and/or
#* modify it under the terms of the GNU General Public License
#* as published by the Free Software Foundation; either version 2
#* of the License, or (at your option) any later version.
#*
#* This program is distributed in the hope that it will be useful,
#* but WITHOUT ANY WARRANTY; without even the implied warranty of
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#* GNU General Public License for more details.
#*
#* You should have received a copy of the GNU General Public License
#* along with this program; if not, write to the Free Software
#* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#*
#*****************************************************************************
set -e

if [ -f wxformbuilder_*.orig.tar.gz ]
then
   echo "Please clean old source tarballs first"
   exit 1
fi

function pause()
{
   read -p “$*”
}

# this function does the actual work of making the source package
# the version is passed to it as the first argument, below
function add_log_entry
{
	set -e

	# use debchange
	echo debchange -D $2 -v "$1-0ubuntu1~${2}1" -m
	debchange -D $2 -v "$1-0ubuntu1~${2}1" -m
}

# Parse command line options
upload=""
source=""
change=""
alter=""
for args in "$@"
do
    if ( [ ${args} = "--help" ] || [ ${args} = "-h" ] ); then
        echo "Available options:"
        echo
	# echo "-c, --c		Update changelog"
        echo "-s, --s		Create source package only, otherwise also binary package will be created"
	echo "-a, --a		Create alternative package only (orig. source wont be uploaded)"
        echo "-u, --u		Upload package to PPA"
        echo
        exit
    elif ( [ ${args} = "-u" ] || [ ${args} = "--u" ] ); then
        upload="1"
        continue
    elif ( [ ${args} = "-s" ] || [ ${args} = "--s" ] ); then
        source="1"
        continue
    elif ( [ ${args} = "-c" ] || [ ${args} = "--c" ] ); then
        change="1"
        continue
    elif ( [ ${args} = "-a" ] || [ ${args} = "--a" ] ); then
        alter="1"
        continue
    fi
done

# get source tarball
debian/rules get-orig-source

#extract it
tar xzf wxformbuilder_*.orig.tar.gz

#save current dir
currentdir=`pwd`

#save the source dir
cd wxformbuilder-*
sourcedir=`pwd`

#copy the debian directory to the correct directory
cp -R install/linux/debian .

#update the changelog by hand for now
nano debian/changelog
#cp -f debian/changelog $currentdir/debian/changelog

#if ( [ -n "$change" ] ); then
#    echo "Enter distribution: "
#    read dist
#
#    #determine version
#    changelog="changes.txt"
#    if [ ! -f $changelog ];
#    then
#        echo "Sorry, could not find "$changelog". Need it to parse the version."
#        exit 1
#    fi
#
#    cat "$changelog" |
#    while read line;
#    do
#        version=`expr match "$line" '.*\([0-9]\.[0-9]\{1,2\}\.[0-9]\+\)'`
#        if [ -n "$version" ]
#        then
#	    echo "Detected version: "
#            echo "$version"
#            # because I redirected cat to the while loop, bash spawned a subshell
#            # this means "version" will go out of scope at the end of the loop
#            # so I need to do everything here
#            if [ ${#version} -ge 7 ]
#            then
#                version=${version/0/}
#            fi
#            add_log_entry $version $dist
#            break
#        fi
#    done
#
#    cp -f debian/changelog $currentdir/debian/changelog
#fi

#make debian archives
if ( [ -n "$alter" ] ); then
    dpkg-buildpackage -S -sd -rfakeroot
elif ( [ -n "$source" ] ); then
    dpkg-buildpackage -S -sa -rfakeroot
else
    dpkg-buildpackage -rfakeroot
fi

#cleanup
cd $currentdir
rm -r $sourcedir

#upload the files to the PPA
if ( [ -n "$upload" ] ); then
    dput ppa:michal-bliznak-tbu/wxformbuilder *.changes
fi
