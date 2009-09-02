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

APPCONTENTS=./output/wxFormBuilder.app/Contents

cd ..
	
rm -r -f ./output/wxFormBuilder.app
mkdir ./output/wxFormBuilder.app	
mkdir $APPCONTENTS
mkdir $APPCONTENTS/Resources
mkdir $APPCONTENTS/MacOS
mkdir $APPCONTENTS/PlugIns
mkdir $APPCONTENTS/SharedSupport
if [ -f ./output/bin/wxformbuilder ]; then
	cp ./output/bin/wxformbuilder $APPCONTENTS/MacOS/wxformbuilder
else
	cp ./output/bin/wxformbuilder.app/Contents/MacOS/wxformbuilder $APPCONTENTS/MacOS/wxformbuilder
fi
cp ./output/lib/wxformbuilder/* $APPCONTENTS/PlugIns
cp -r ./output/plugins/ $APPCONTENTS/SharedSupport/plugins
cp -r ./output/resources/ $APPCONTENTS/SharedSupport/resources
cp -r ./output/xml/ $APPCONTENTS/SharedSupport/xml
cp ./install/macosx/icon.icns $APPCONTENTS/Resources/icon.icns
cp ./install/macosx/docicon.icns $APPCONTENTS/Resources/docicon.icns
cp ./install/macosx/Info.plist $APPCONTENTS/Info.plist

# fix libraries' internal name and path... not really necessary but better
install_name_tool -id @executable_path/../PlugIns/libwx_macu_flatnotebook-2.8_wxfb.dylib $APPCONTENTS/PlugIns/libwx_macu_flatnotebook-2.8_wxfb.dylib
install_name_tool -id @executable_path/../PlugIns/libwx_macu_propgrid-2.8_wxfb.dylib $APPCONTENTS/PlugIns/libwx_macu_propgrid-2.8_wxfb.dylib
install_name_tool -id @executable_path/../PlugIns/libwx_macu_scintilla-2.8_wxfb.dylib $APPCONTENTS/PlugIns/libwx_macu_scintilla-2.8_wxfb.dylib
install_name_tool -id @executable_path/../PlugIns/libwxadditions-mini.dylib $APPCONTENTS/PlugIns/libwxadditions-mini.dylib
install_name_tool -id @executable_path/../PlugIns/libadditional.dylib $APPCONTENTS/PlugIns/libadditional.dylib
install_name_tool -id @executable_path/../PlugIns/libcommon.dylib $APPCONTENTS/PlugIns/libcommon.dylib
install_name_tool -id @executable_path/../PlugIns/libcontainers.dylib $APPCONTENTS/PlugIns/libcontainers.dylib
install_name_tool -id @executable_path/../PlugIns/liblayout.dylib $APPCONTENTS/PlugIns/liblayout.dylib

# fix links betwen libraries now that we moved them, so they can find each other
# in their new locations
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macu_flatnotebook-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macu_flatnotebook-2.8_wxfb.dylib $APPCONTENTS/MacOS/wxformbuilder
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macu_propgrid-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macu_propgrid-2.8_wxfb.dylib $APPCONTENTS/MacOS/wxformbuilder
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macu_scintilla-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macu_scintilla-2.8_wxfb.dylib $APPCONTENTS/MacOS/wxformbuilder

install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macu_flatnotebook-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macu_flatnotebook-2.8_wxfb.dylib $APPCONTENTS/PlugIns/libwxadditions-mini.dylib
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macu_propgrid-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macu_propgrid-2.8_wxfb.dylib $APPCONTENTS/PlugIns/libwxadditions-mini.dylib
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macu_scintilla-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macu_scintilla-2.8_wxfb.dylib $APPCONTENTS/PlugIns/libwxadditions-mini.dylib
