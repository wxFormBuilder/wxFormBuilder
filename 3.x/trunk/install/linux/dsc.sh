#!/bin/bash

# this function does the actual work of making the source package
# the version is passed to it as the first argument, below
function make_dsc
{
	# root source dir
	basedir=.

	# controls dir
	controls_h=src/controls/include/wx
	controls_c=src/controls/src

	# create a temporary directory
	tmpdir=wxformbuilder-$1
	mkdir $tmpdir
	if [ ! -d $tmpdir ]
	then
	  echo "Unable to create temporary directory \"$tmpdir\""
	  exit 1
	fi

	svn export $basedir/output   $tmpdir/output
	rm $tmpdir/output/license.txt
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
   make_dsc $version
   break
 fi
done

exit
