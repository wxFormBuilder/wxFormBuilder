-----------------------------------------------------------------------------
--  Name:        common.lua
--  Purpose:     Common controls plugin project build script.
--  Author:      Andrea Zanellato
--  Modified by: 
--  Created:     21/10/2011
--  Copyright:   (c) 2011 wxFormBuilder Team
--  Licence:     GNU General Public License Version 2
-----------------------------------------------------------------------------
project "common-components-plugin"
    kind                "SharedLib"
    targetname          "common"
    files               {"../../../plugins/common/common.cpp"}
    includedirs
    {
        "../../../sdk/tinyxml", "../../../sdk/plugin_interface"
    }
    defines             {"BUILD_DLL", "TIXML_USE_TICPP"}
    flags               {"ExtraWarnings"}
    links               {"plugin-interface", "TiCPP"}
	
	if wxArchitecture then
		buildoptions	{"-arch " .. wxArchitecture}
	end

    configuration "not windows"
        targetdir       "../../../output/lib/wxformbuilder"

    configuration "windows"
        targetprefix    "lib"
        targetdir       "../../../output/plugins/common"

    configuration "Debug"
        targetsuffix    ( DebugSuffix )
        wx_config       { Debug="yes" }

    configuration "Release"
        buildoptions    {"-fno-strict-aliasing"}
        wx_config       {}
