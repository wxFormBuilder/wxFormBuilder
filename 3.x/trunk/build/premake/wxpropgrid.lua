-----------------------------------------------------------------------------
--  Name:        wxpropgrid.lua
--  Purpose:     wxPropertyGrid library build script for wxWidgets 2.8.
--  Author:      Andrea Zanellato
--  Modified by: 
--  Created:     19/10/2011
--  Copyright:   (c) 2011 wxFormBuilder Team
--  Licence:     GNU General Public License Version 2
-----------------------------------------------------------------------------
project "wxPropertyGrid"
    kind                "SharedLib"
    files
    {
        "../../src/controls/src/propgrid/*.cpp",
        "../../src/controls/include/wx/propgrid/*.h"
    }
    includedirs         {"../../src/controls/include"}
    defines             {"WXMAKINGDLL_PROPGRID"}
    flags               {"ExtraWarnings"}
    targetsuffix        ( "-" .. wxVersion .. "_wxfb" )
	
	if wxArchitecture then
		buildoptions	{"-arch " .. wxArchitecture}
	end

    configuration "not windows"
        targetdir       "../../output/lib/wxformbuilder"

    configuration "windows"
        targetdir       "../../output"

    configuration "Release"
        buildoptions    {"-fno-strict-aliasing"}
        targetname      ( CustomPrefix .. "_propgrid" )
        wx_config       {}

    configuration "Debug"
        targetname      ( CustomPrefix .. wxDebugSuffix .. "_propgrid" )
        wx_config       { Debug="yes" }
