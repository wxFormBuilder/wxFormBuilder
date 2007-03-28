#!/bin/bash

# this function does the actual work of copying files and archiving
# the version is passed to it as the first argument, below
function archive
{
  version=$1
  outputDir=../wxfb
  name="wxFormBuilder_v"$version"-beta3.tar.bz2"
  
  rm -r $outputDir
  svn export bin $outputDir
  cp bin/wxFormBuilder $outputDir
  cp -R bin/lib $outputDir
  rm $outputDir/lib/*d.so
  cp bin/plugins/additional/libadditional.so $outputDir/plugins/additional
  cp bin/plugins/common/libcommon.so $outputDir/plugins/common
  cp bin/plugins/layout/liblayout.so $outputDir/plugins/layout
  cp bin/plugins/containers/libcontainers.so $outputDir/plugins/containers
  cp bin/plugins/wxAdditions/libwxadditions\-mini.so $outputDir/plugins/wxAdditions
  cp /opt/wx/2.8.3/lib/libwx_gtk2u-2.8.so.0.1.1 $outputDir/lib/libwx_gtk2u-2.8.so.0 
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

