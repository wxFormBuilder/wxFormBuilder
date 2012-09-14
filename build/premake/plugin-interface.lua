-----------------------------------------------------------------------------
--  Name:        plugin_interface.lua
--  Purpose:     Plugin Interface library project script.
--  Author:      Andrea Zanellato zanellato.andrea@gmail.com
--  Modified by:
--  Created:     2011/10/20
--  Copyright:   (c) wxFormBuilder Team
--  Licence:     GNU General Public License Version 2
-----------------------------------------------------------------------------
project "plugin-interface"
    kind                "StaticLib"
    files
    {
        "../../sdk/plugin_interface/**.h",
        "../../sdk/plugin_interface/**.cpp",
        "../../sdk/plugin_interface/**.fbp"
    }
    includedirs         {"../../sdk/tinyxml"}
    targetdir           "../../sdk/lib"
    flags               {"ExtraWarnings"}
    defines             {"TIXML_USE_TICPP"}
    targetsuffix        ( "-" .. wxVersion )


configuration "not codelite"
	links               {"TiCPP"}
	
configuration {"codelite", "Release"}
	links				{"../../sdk/lib/libticpp.a"}
	
configuration {"codelite", "Debug"}
	links				{"../../sdk/lib/libticppd.a"}

configuration "not windows"
    buildoptions {"-fPIC"}

 -- Visual C++ 2005/2008
configuration "vs*"
    defines             {"_CRT_SECURE_NO_DEPRECATE"}

configuration "Debug"
    targetname          ( CustomPrefix .. wxDebugSuffix .. "_plugin-interface" )
    wx_config           { Debug="yes" }

configuration "Release"
    buildoptions        {"-fno-strict-aliasing"}
    targetname          ( CustomPrefix .. "_plugin-interface" )
    wx_config           {}


