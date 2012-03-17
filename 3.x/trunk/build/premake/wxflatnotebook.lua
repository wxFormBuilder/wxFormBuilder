-----------------------------------------------------------------------------
--  Name:        wxflatnotebook.lua
--  Purpose:     wxFlatNotebook library build script.
--  Author:      Andrea Zanellato
--  Modified by: 
--  Created:     21/10/2011
--  Copyright:   (c) 2011 wxFormBuilder Team
--  Licence:     GNU General Public License Version 2
-----------------------------------------------------------------------------
project "wxFlatNotebook"
    kind                "SharedLib"
    files
    {
        "../../src/controls/src/wxFlatNotebook/*.cpp",
        "../../src/controls/include/wx/wxFlatNotebook/*.h"
    }
    includedirs         {"../../src/controls/include"}
    defines             {"WXMAKINGDLL_FNB"}
    flags               {"ExtraWarnings"}
    targetsuffix        ( "-" .. wxVersion .. "_wxfb" )

    configuration "not windows"
        targetdir       "../../output/lib/wxformbuilder"

    configuration "windows"
        targetdir       "../../output"

    configuration "Release"
        buildoptions    {"-fno-strict-aliasing"}
        targetname      ( CustomPrefix .. "_flatnotebook" )
        wx_config       {}

    configuration "Debug"
        targetname      ( CustomPrefix .. wxDebugSuffix .. "_flatnotebook" )
        wx_config       { Debug="yes" }
