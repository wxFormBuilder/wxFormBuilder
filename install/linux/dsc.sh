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

if [ -f wxformbuilder-*.orig.tar.gz ]
then
   echo "Please clean old source tarballs first"
   exit 1
fi

# get source tarball
debian/rules get-orig-source

#extract it
tar xzf wxformbuilder_*.orig.tar.gz

#determine version
changelog=`cd wxformbuilder_*.orig/install/linux && dpkg-parsechangelog`
version=`expr match "$changelog" '.*Version: \([0-9]\.[0-9]\{1,2\}\.[0-9]\+\).*'`

#rename it
sourcedir=wxformbuilder-$version
mv wxformbuilder_*.orig $sourcedir
cp -R $sourcedir/install/linux/debian $sourcedir/debian

#save current dir
currentdir=`pwd`

#enter source dir
cd $sourcedir

#make debian source archive
dpkg-buildpackage -S -sa -rfakeroot

#cleanup
cd $currentdir
rm -r $sourcedir
