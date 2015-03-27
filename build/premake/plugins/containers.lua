-----------------------------------------------------------------------------
--  Name:        containers.lua
--  Purpose:     Container controls plugin project build script.
--  Author:      Andrea Zanellato
--  Modified by: 
--  Created:     21/10/2011
--  Copyright:   (c) 2011 wxFormBuilder Team
--  Licence:     GNU General Public License Version 2
-----------------------------------------------------------------------------
project "containers-components-plugin"
    kind                "SharedLib"
    targetname          "containers"
    files               {"../../../plugins/containers/containers.cpp"}
    includedirs
    {
        "../../../sdk/tinyxml", "../../../sdk/plugin_interface"
    }
    defines             {"BUILD_DLL", "TIXML_USE_TICPP"}
    flags               {"ExtraWarnings"}
    links               {"plugin-interface", "TiCPP"}

    local libs = "std,richtext,propgrid,stc,ribbon,aui"

	if wxArchitecture then
		buildoptions	{"-arch " .. wxArchitecture}
	end

    configuration "not windows"
        targetdir       "../../../output/lib/wxformbuilder"

    configuration "windows"
        targetprefix    "lib"
        targetdir       "../../../output/plugins/containers"

    configuration "Debug"
        targetsuffix    ( DebugSuffix )
        wx_config       { Debug="yes", Libs=libs }

    configuration "Release"
        buildoptions    {"-fno-strict-aliasing"}
        wx_config       { Libs=libs }
