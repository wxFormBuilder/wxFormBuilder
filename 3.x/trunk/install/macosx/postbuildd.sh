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
#* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#*
#*****************************************************************************

APPCONTENTS=./output/wxFormBuilderd.app/Contents

cd ..
	
rm -r -f ./output/wxFormBuilderd.app
mkdir ./output/wxFormBuilderd.app	
mkdir $APPCONTENTS
mkdir $APPCONTENTS/Resources
mkdir $APPCONTENTS/MacOS
mkdir $APPCONTENTS/PlugIns
mkdir $APPCONTENTS/SharedSupport
if [ -f ./output/bin/wxformbuilderd ]; then
	cp ./output/bin/wxformbuilderd $APPCONTENTS/MacOS/wxformbuilderd
else
	cp ./output/bin/wxformbuilderd.app/Contents/MacOS/wxformbuilderd $APPCONTENTS/MacOS/wxformbuilderd
fi
cp ./output/lib/wxformbuilder/* $APPCONTENTS/PlugIns
cp -r ./output/plugins/ $APPCONTENTS/SharedSupport/plugins
cp -r ./output/resources/ $APPCONTENTS/SharedSupport/resources
cp -r ./output/xml/ $APPCONTENTS/SharedSupport/xml
cp ./install/macosx/icon.icns $APPCONTENTS/Resources/icon.icns
cp ./install/macosx/docicon.icns $APPCONTENTS/Resources/docicon.icns
cp ./install/macosx/Info.plist $APPCONTENTS/Info.plist

# fix libraries' internal name and path... not really necessary but better
install_name_tool -id @executable_path/../PlugIns/libwx_macud_flatnotebook-2.8_wxfb.dylib $APPCONTENTS/PlugIns/libwx_macud_flatnotebook-2.8_wxfb.dylib
install_name_tool -id @executable_path/../PlugIns/libwx_macud_propgrid-2.8_wxfb.dylib $APPCONTENTS/PlugIns/libwx_macud_propgrid-2.8_wxfb.dylib
install_name_tool -id @executable_path/../PlugIns/libwx_macud_scintilla-2.8_wxfb.dylib $APPCONTENTS/PlugIns/libwx_macud_scintilla-2.8_wxfb.dylib
install_name_tool -id @executable_path/../PlugIns/libwxadditions-minid.dylib $APPCONTENTS/PlugIns/libwxadditions-minid.dylib
install_name_tool -id @executable_path/../PlugIns/libadditionald.dylib $APPCONTENTS/PlugIns/libadditionald.dylib
install_name_tool -id @executable_path/../PlugIns/libcommond.dylib $APPCONTENTS/PlugIns/libcommond.dylib
install_name_tool -id @executable_path/../PlugIns/libcontainersd.dylib $APPCONTENTS/PlugIns/libcontainersd.dylib
install_name_tool -id @executable_path/../PlugIns/liblayoutd.dylib $APPCONTENTS/PlugIns/liblayoutd.dylib

# fix links betwen libraries now that we moved them, so they can find each other
# in their new locations
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macud_flatnotebook-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macud_flatnotebook-2.8_wxfb.dylib $APPCONTENTS/MacOS/wxformbuilderd
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macud_propgrid-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macud_propgrid-2.8_wxfb.dylib $APPCONTENTS/MacOS/wxformbuilderd
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macud_scintilla-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macud_scintilla-2.8_wxfb.dylib $APPCONTENTS/MacOS/wxformbuilderd

install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macud_flatnotebook-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macud_flatnotebook-2.8_wxfb.dylib $APPCONTENTS/PlugIns/libwxadditions-minid.dylib
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macud_propgrid-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macud_propgrid-2.8_wxfb.dylib $APPCONTENTS/PlugIns/libwxadditions-minid.dylib
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macud_scintilla-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macud_scintilla-2.8_wxfb.dylib $APPCONTENTS/PlugIns/libwxadditions-minid.dylib
