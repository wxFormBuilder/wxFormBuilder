#!/bin/bash

if [ ! -n "$1" ]
then
 echo "Please specify output directory."
 exit
else
 outputDir=$1
fi  
  
if [ ! -d $outputDir ]
then
  mkdir $outputDir
  mkdir $outputDir/bin
  mkdir $outputDir/share
  mkdir $outputDir/share/wxformbuilder
fi

if [ ! -d $outputDir/bin ]
then
  mkdir $outputDir/bin
fi
  
if [ ! -d $outputDir/share ]
then
  mkdir $outputDir/share
  mkdir $outputDir/share/wxformbuilder
fi
  
if [ ! -d $outputDir/share/wxformbuilder ]
then
  mkdir $outputDir/share/wxformbuilder
fi
  
cp -R --interactive --verbose ../../output/* $outputDir/share/wxformbuilder/

if [ -d $outputDir/bin ]
then
  mv -f -v $outputDir/share/wxformbuilder/bin/* $outputDir/bin
  rm -r $outputDir/share/wxformbuilder/bin
else
  mv -f -v $outputDir/share/wxformbuilder/bin $outputDir/
fi

if [ -d $outputDir/lib ]
then
  mv -f -v $outputDir/share/wxformbuilder/lib/* $outputDir/lib
  rm - r $outputDir/share/wxformbuilder/lib
else
 mv -f -v $outputDir/share/wxformbuilder/lib $outputDir/
fi

if [ -d $outputDir/share/wxformbuilder/share ]
then
 rm -r $outputDir/share/wxformbuilder/share
fi

exit

