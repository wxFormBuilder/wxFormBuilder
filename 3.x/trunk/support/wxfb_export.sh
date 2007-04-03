#!/bin/bash

if [ ! -n "$1" ]
then
 outputDir=../wxfb
else
 outputDir=$1
fi  
  
dataDir=$outputDir/share/wxformbuilder
  
  if [ ! -d $outputDir ]
  then
    mkdir $outputDir
  fi
  
  if [ ! -d $outputDir/share ]
  then
    mkdir $outputDir/share
  fi
  
  if [ ! -d $dataDir ]
  then
    mkdir $dataDir
  fi
  
  if [ ! -d $outputDir/bin ]
  then
    mkdir $outputDir/bin
  fi
 
  svn export --force bin $dataDir
  cp bin/wxFormBuilder $outputDir/bin
  cp -R bin/lib $outputDir
  rm $outputDir/lib/*d.so
  cp bin/plugins/additional/libadditional.so $dataDir/plugins/additional
  cp bin/plugins/common/libcommon.so $dataDir/plugins/common
  cp bin/plugins/layout/liblayout.so $dataDir/plugins/layout
  cp bin/plugins/containers/libcontainers.so $dataDir/plugins/containers
  cp bin/plugins/wxAdditions/libwxadditions\-mini.so $dataDir/plugins/wxAdditions

exit

