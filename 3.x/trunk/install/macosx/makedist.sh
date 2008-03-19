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

# uses tool macdylibbundler.sf.net to integrate wxWidgets libraries
# inside the app bundle. Not necessary if you require the user to install
# them on his side, or if you use the ones that come with the system on Leopard
#
# will also be different if you built monolithic

dylibbundler -c ./install/macosx/dylibbundler.conf -l -x ./output/wxFormBuilder.app/Contents/MacOS/wxformbuilder -d ./output/wxFormBuilder.app/Contents/libs/ -cd -od
dylibbundler -c ./install/macosx/dylibbundler.conf -x ./output/wxFormBuilder.app/Contents/PlugIns/libadditional.dylib
dylibbundler -c ./install/macosx/dylibbundler.conf -x ./output/wxFormBuilder.app/Contents/PlugIns/libcommon.dylib
dylibbundler -c ./install/macosx/dylibbundler.conf -x ./output/wxFormBuilder.app/Contents/PlugIns/libcontainers.dylib
dylibbundler -c ./install/macosx/dylibbundler.conf -x ./output/wxFormBuilder.app/Contents/PlugIns/liblayout.dylib
dylibbundler -c ./install/macosx/dylibbundler.conf -x ./output/wxFormBuilder.app/Contents/PlugIns/libwx_macu_flatnotebook-2.8_wxfb.dylib
dylibbundler -c ./install/macosx/dylibbundler.conf -x ./output/wxFormBuilder.app/Contents/PlugIns/libwx_macu_propgrid-2.8_wxfb.dylib
dylibbundler -c ./install/macosx/dylibbundler.conf -x ./output/wxFormBuilder.app/Contents/PlugIns/libwx_macu_scintilla-2.8_wxfb.dylib
dylibbundler -c ./install/macosx/dylibbundler.conf -x ./output/wxFormBuilder.app/Contents/PlugIns/libwxadditions-mini.dylib
