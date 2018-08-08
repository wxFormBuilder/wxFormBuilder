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
    includedirs         {"../../subprojects/ticpp"}
    targetdir           "../../sdk/lib"
    defines             {"TIXML_USE_TICPP"}
    targetsuffix        ( "-" .. wxVersion )

	if wxArchitecture then
		buildoptions	{"-arch " .. wxArchitecture}
	end

    configuration "not vs*"
        buildoptions        "-std=c++14"

configuration "not windows"
    buildoptions {"-fPIC"}

 -- Visual C++ 2005/2008
configuration "vs*"
    defines             {"_CRT_SECURE_NO_DEPRECATE", "_CRT_SECURE_NO_WARNINGS"}

configuration "Debug"
    targetname          ( CustomPrefix .. wxDebugSuffix .. "_plugin-interface" )
    wx_config           { Debug="yes", WithoutLibs="yes" }

configuration "Release"
    targetname          ( CustomPrefix .. "_plugin-interface" )
    wx_config           { WithoutLibs="yes" }

    configuration {"not vs*", "Release"}
        buildoptions    {"-fno-strict-aliasing"}
