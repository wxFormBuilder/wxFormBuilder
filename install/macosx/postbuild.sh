#!/bin/sh
#*****************************************************************************
#* Copyright (C) 2007 Auria
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
#* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#*
#*****************************************************************************

usage() { echo "Usage: $0 -c (debug|release) " 1>&2; exit 1; }

while getopts ":c:" c; do
    case "${c}" in
        c)
			CONFIG="${OPTARG}"
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))


if [ "debug" != "$CONFIG" ] && [ "release" != "$CONFIG" ]; then
    usage
fi

# reliable way to get the absolute path inside a bash script
# per http://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
pushd `dirname $0` > /dev/null
cd ../../
PROJECT_ROOT=`pwd`
popd > /dev/null

if [ "$CONFIG" == "debug" ]; then
	APPCONTENTS="$PROJECT_ROOT/output/wxFormBuilderd.app/Contents"
	rm -r -f $PROJECT_ROOT/output/wxFormBuilderd.app
	mkdir -p $PROJECT_ROOT/output/wxFormBuilderd.app
else 
	APPCONTENTS="$PROJECT_ROOT/output/wxFormBuilder.app/Contents"
	rm -r -f $PROJECT_ROOT/output/wxFormBuilder.app
	mkdir -p $PROJECT_ROOT/output/wxFormBuilder.app
fi

mkdir -p $APPCONTENTS
mkdir -p $APPCONTENTS/Resources
mkdir -p $APPCONTENTS/MacOS
mkdir -p $APPCONTENTS/PlugIns
mkdir -p $APPCONTENTS/SharedSupport

# read carefully, the original output by the compiler is itself an app bundle 
# located in output/bin/, but we create our own directory for another app 
# bundle
if [ -f $PROJECT_ROOT/output/bin/wxformbuilder.app/Contents/MacOS/wxformbuilder-2.8 ]; then
	cp $PROJECT_ROOT/output/bin/wxformbuilder.app/Contents/MacOS/wxformbuilder-2.8 $APPCONTENTS/MacOS/wxformbuilder
fi
if [ -f $PROJECT_ROOT/output/bin/wxformbuilder.app/Contents/MacOS/wxformbuilder-2.9 ]; then
	cp $PROJECT_ROOT/output/bin/wxformbuilder.app/Contents/MacOS/wxformbuilder-2.9 $APPCONTENTS/MacOS/wxformbuilder
fi
if [ -f $PROJECT_ROOT/output/bin/wxformbuilder.app/Contents/MacOS/wxformbuilder-3.1 ]; then
	cp $PROJECT_ROOT/output/bin/wxformbuilder.app/Contents/MacOS/wxformbuilder-3.1 $APPCONTENTS/MacOS/wxformbuilder
fi
if [ -f $PROJECT_ROOT/output/bin/wxformbuilder.app/Contents/MacOS/wxformbuilder ]; then
	cp $PROJECT_ROOT/output/bin/wxformbuilder.app/Contents/MacOS/wxformbuilder $APPCONTENTS/MacOS/wxformbuilder
fi

cp $PROJECT_ROOT/output/lib/wxformbuilder/* $APPCONTENTS/PlugIns
cp -r $PROJECT_ROOT/output/plugins/ $APPCONTENTS/SharedSupport/plugins
cp -r $PROJECT_ROOT/output/resources/ $APPCONTENTS/SharedSupport/resources
cp -r $PROJECT_ROOT/output/xml/ $APPCONTENTS/SharedSupport/xml

cp $PROJECT_ROOT/install/macosx/icon.icns $APPCONTENTS/Resources/icon.icns
cp $PROJECT_ROOT/install/macosx/docicon.icns $APPCONTENTS/Resources/docicon.icns
cp $PROJECT_ROOT/install/macosx/Info.plist $APPCONTENTS/Info.plist

# fix libraries' internal name and path
# we set the ID to be the full path, so that the dylibbundler tool
# can find them when we make an app bundle
for filename in $APPCONTENTS/PlugIns/*.dylib; do
	base=`basename "$filename"`
	`install_name_tool -id "$APPCONTENTS/PlugIns/$base" "$APPCONTENTS/PlugIns/$base"`
	codesign -s - -f --verbose "$filename"
done

if [ "$CONFIG" == "release" ]; then
	sh $PROJECT_ROOT/install/macosx/makedist.sh
fi
