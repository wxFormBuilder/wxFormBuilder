#!/bin/bash

# this function does the actual work of copying files and archiving
# the version is passed to it as the first argument, below
function archive
{
  outputDir=../wxfb

  # export to directory
  if [ -d $outputDir ]
  then
   rm -r $outputDir
  fi
  support/wxfb_export.sh $outputDir
  
  name="wxFormBuilder_v"$1"-beta3.tar.bz2"
  
  cp /opt/wx/2.8.3/lib/libwx_gtk2u-2.8.so.0.1.1 $outputDir/lib/libwx_gtk2u-2.8.so.0 
  ln -s ../lib $outputDir/bin/lib
  if [ -f $name ]
  then
    rm ../$name
  fi
  tar cjf ../$name $outputDir
}

changelog="bin/Changelog.txt"

if [ ! -f $changelog ];
then
  echo "Sorry, could not find "$changelog". Need it to parse the version."
  exit 1
fi


versionRegEx="[0-9]\.[0-9]{1,2}\.[0-9]+"

cat "$changelog" | 
while read line;
do
 if [[ "$line" =~ ".*Version "$versionRegEx".*" ]]
 then
   version=`expr match "$line" '.*\([0-9]\.[0-9]\{1,2\}\.[0-9]\+\)'`
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

