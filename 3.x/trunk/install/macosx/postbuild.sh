#!/bin/sh

export TOFILE=./output/wxFormBuilder.app/Contents/info.plist

cd ..
	
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macu_flatnotebook-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macu_flatnotebook-2.8_wxfb.dylib ./output/bin/wxformbuilder
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macu_propgrid-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macu_propgrid-2.8_wxfb.dylib ./output/bin/wxformbuilder
install_name_tool -change ../../../../output/lib/wxformbuilder/libwx_macu_scintilla-2.8_wxfb.dylib @executable_path/../PlugIns/libwx_macu_scintilla-2.8_wxfb.dylib ./output/bin/wxformbuilder
install_name_tool -id @executable_path/../PlugIns/libwx_macu_flatnotebook-2.8_wxfb.dylib ./output/lib/wxformbuilder/libwx_macu_flatnotebook-2.8_wxfb.dylib
install_name_tool -id @executable_path/../PlugIns/libwx_macu_propgrid-2.8_wxfb.dylib ./output/lib/wxformbuilder/libwx_macu_propgrid-2.8_wxfb.dylib
install_name_tool -id @executable_path/../PlugIns/libwx_macu_scintilla-2.8_wxfb.dylib ./output/lib/wxformbuilder/libwx_macu_scintilla-2.8_wxfb.dylib

rm -r -f ./output/wxFormBuilder.app
mkdir ./output/wxFormBuilder.app	
mkdir ./output/wxFormBuilder.app/Contents
mkdir ./output/wxFormBuilder.app/Contents/Resources
mkdir ./output/wxFormBuilder.app/Contents/MacOS
mkdir ./output/wxFormBuilder.app/Contents/PlugIns
mkdir ./output/wxFormBuilder.app/Contents/SharedSupport
cp ./output/bin/wxformbuilder ./output/wxFormBuilder.app/Contents/MacOS/wxformbuilder
cp ./output/lib/wxformbuilder/* ./output/wxFormBuilder.app/Contents/PlugIns
cp -r ./output/plugins/ ./output/wxFormBuilder.app/Contents/SharedSupport/plugins
cp -r ./output/resources/ ./output/wxFormBuilder.app/Contents/SharedSupport/resources
cp -r ./output/xml/ ./output/wxFormBuilder.app/Contents/SharedSupport/xml
cp ./install/macosx/icon.icns ./output/wxFormBuilder.app/Contents/Resources/icon.icns
cp ./install/macosx/docicon.icns ./output/wxFormBuilder.app/Contents/Resources/docicon.icns
cp ./install/macosx/Info.plist ./output/wxFormBuilder.app/Contents/Info.plist
