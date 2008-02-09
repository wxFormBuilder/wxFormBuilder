#!/bin/sh
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

