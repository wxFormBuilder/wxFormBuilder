-----------------------------------------------------------------------------
--  Name:        ticpp.lua
--  Purpose:     TinyXML project script.
--  Author:      RJP Computing <rjpcomputing@gmail.com>
--  Modified by: Andrea Zanellato zanellato.andrea@gmail.com
--  Created:     2008/21/01
--  Copyright:   (c) 2009 RJP Computing
--  Licence:     GNU General Public License Version 2
-----------------------------------------------------------------------------
project "TiCPP"
    kind                "StaticLib"
    targetname          "ticpp"
    targetdir           "../../sdk/lib"
    files               {"../../sdk/tinyxml/*.cpp", "../../sdk/tinyxml/*.h"}
    excludes            {"xmltest.cpp"}
    defines             {"TIXML_USE_TICPP"}
	
	if wxArchitecture then
		buildoptions	{"-arch " .. wxArchitecture}
	end

    configuration "not vs*"
        buildoptions        "-std=c++14"

    configuration "not windows"
        buildoptions    {"-fPIC"}

    configuration "vs*"
        defines         {"_CRT_SECURE_NO_DEPRECATE", "_CRT_SECURE_NO_WARNINGS"}

    configuration "Debug"
        targetsuffix    "d"
