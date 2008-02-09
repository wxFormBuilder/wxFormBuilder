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
# this function does the actual work of copying files and archiving
# the version is passed to it as the first argument, below
function archive
{
  set -e
  
  # copy monolithic wx lib to lib dir
  cp -L `wx-config --prefix`/lib/libwx_gtk2u-2.8.so.0 output/lib/wxformbuilder/

  # remove the share/wxformbuilder symlink
  rm output/share/wxformbuilder
  rm output/share/README.txt

  # copy the output directory to the share directory
  mkdir output/share/wxformbuilder
  mv -t output/share/wxformbuilder/ output/xml
  mv -t output/share/wxformbuilder/ output/resources
  mv -t output/share/wxformbuilder/ output/plugins
  mv -t output/share/wxformbuilder/ output/*.*

  # rename the output folder for tar
  mv output wxformbuilder

  # create archive
  name="wxFormBuilder_v"$1"-$2.tar.bz2"
  if [ -f $name ]
  then
    rm $name
  fi
  tar cjf $name wxformbuilder
}

changelog="output/Changelog.txt"

if [ ! -f $changelog ];
then
  echo "Sorry, could not find "$changelog". Need it to parse the version."
  exit 1
fi

cat "$changelog" |
while read line;
do
 version=`expr match "$line" '.*\([0-9]\.[0-9]\{1,2\}\.[0-9]\+\)'`
 if [ -n "$version" ]
 then
 echo "$version"
   # because I redirected cat to the while loop, bash spawned a subshell
   # this means "version" will go out of scop at the end of the loop
   # so I need to do everything here
   if [ ${#version} -ge 7 ]
   then
     version=${version/0/}
   fi
   label=`expr match "$line" '.*(\(.*\))'`
   archive $version $label
   break
 fi
done

exit

