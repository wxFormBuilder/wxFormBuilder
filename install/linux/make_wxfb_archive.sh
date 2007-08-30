#!/bin/bash

# this function does the actual work of copying files and archiving
# the version is passed to it as the first argument, below
function archive
{
  # copy monolithic wx lib to lib dir
  cp /opt/wx/2.8.4/lib/libwx_gtk2u-2.8.so.0.1.1 output/lib/libwx_gtk2u-2.8.so.0

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
  name="wxFormBuilder_v"$1"-beta3.tar.bz2"
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
   archive $version
   break
 fi
done

exit

