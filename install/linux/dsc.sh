#!/bin/sh

# root source dir
basedir=.

# controls dir
controls_h=src/controls/include/wx
controls_c=src/controls/src

# create a temporary directory
tmpdir=`date +%m%d%Y%H%M%S`
mkdir $tmpdir
if [ ! -d $tmpdir ]
then
  echo "Unable to create temporary directory \"$tmpdir\""
  exit 1
fi

svn export $basedir/output   $tmpdir/output
svn export $basedir/plugins  $tmpdir/plugins
svn export $basedir/sdk      $tmpdir/sdk
svn export $basedir/sdk/tinyxml $tmpdir/sdk/tinyxml
svn export $basedir/src      $tmpdir/src
svn export $basedir/$controls_h/propgrid $tmpdir/$controls_h/propgrid
svn export $basedir/$controls_c/propgrid $tmpdir/$controls_c/propgrid
svn export $basedir/$controls_h/wxFlatNotebook $tmpdir/$controls_h/wxFlatNotebook
svn export $basedir/$controls_c/wxFlatNotebook $tmpdir/$controls_c/wxFlatNotebook
svn export $basedir/$controls_h/wxScintilla $tmpdir/$controls_h/wxScintilla
svn export $basedir/$controls_c/wxScintilla $tmpdir/$controls_c/wxScintilla

svn export $basedir/install/linux/debian $tmpdir/debian
mkdir $tmpdir/debian/scripts
cp $basedir/install/linux/*.sh $tmpdir/debian/scripts/
rm $tmpdir/debian/*.ex
rm $tmpdir/debian/*.EX

cp $basedir/premake.lua $tmpdir/premake.lua
cp $basedir/README.txt $tmpdir/debian/README

currentdir=`pwd`

cd $tmpdir
debuild -S
cd $currentdir
rm -r $tmpdir

exit
