#!/bin/sh


# Get output dir from caller
if [ ! -n "$1" ]
then
 echo "Please specify output directory."
 exit
else
 outputDir=$1
fi  

# Ensure target directories exist  
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

# copy ouput files to target directories  
cp -R --interactive output/* $outputDir/share/wxformbuilder/

# reorganize target directories
if [ -d $outputDir/bin ]
then
  mv -f $outputDir/share/wxformbuilder/bin/* $outputDir/bin
  rm -r $outputDir/share/wxformbuilder/bin
else
  mv -f $outputDir/share/wxformbuilder/bin $outputDir/
fi

if [ -d $outputDir/lib ]
then
  mv -f $outputDir/share/wxformbuilder/lib/* $outputDir/lib
  rm - r $outputDir/share/wxformbuilder/lib
else
 mv -f $outputDir/share/wxformbuilder/lib $outputDir/
fi

if [ -d $outputDir/share/wxformbuilder/share ]
then
 rm -r $outputDir/share/wxformbuilder/share
fi

exit

