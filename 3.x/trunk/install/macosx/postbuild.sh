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
	
	echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" >> $TOFILE
	echo "<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">" >> $TOFILE
	echo "<plist version=\"1.0\">" >> $TOFILE
	echo "<dict>" >> $TOFILE
	echo "	<key>CFBundleDevelopmentRegion</key>" >> $TOFILE
	echo "	<string>English</string>" >> $TOFILE
	echo "	<key>CFBundleExecutable</key>" >> $TOFILE
	echo "	<string>wxformbuilder</string>" >> $TOFILE
	echo "	<key>CFBundleInfoDictionaryVersion</key>" >> $TOFILE
	echo "	<string>6.0</string>" >> $TOFILE
	echo "	<key>CFBundleName</key>" >> $TOFILE
	echo "	<string>wxFormBuilder</string>" >> $TOFILE
	echo "	<key>CFBundleIconFile</key>" >> $TOFILE
	echo "	<string>icon.icns</string>" >> $TOFILE
	echo "	<key>CFBundlePackageType</key>" >> $TOFILE
	echo "	<string>APPL</string>" >> $TOFILE
	echo "	<key>CFBundleVersion</key>" >> $TOFILE
	echo "	<string>3.0</string>" >> $TOFILE
	echo "	<key>CSResourcesFileMapped</key>" >> $TOFILE
	echo "	<true/>	" >> $TOFILE
	echo "</dict>" >> $TOFILE
	echo "</plist>" >> $TOFILE
