-----------------------------------------------------------------------------
--  Name:        wxadditions-mini.lua
--  Purpose:     wxAdditions mini plugin project build script.
--  Author:      Andrea Zanellato
--  Modified by: 
--  Created:     21/10/2011
--  Copyright:   (c) 2011 wxFormBuilder Team
--  Licence:     GNU General Public License Version 2
-----------------------------------------------------------------------------
project "wxadditions-mini-plugin"
    kind                "SharedLib"
    targetname          "wxadditions-mini"
    files
    {
        "../../../plugins/wxAdditions/wxAdditions_mini.cpp"
    }
    includedirs
    {
        "../../../src/controls/include",
        "../../../sdk/tinyxml", "../../../sdk/plugin_interface"
    }
    defines             {"BUILD_DLL", "TIXML_USE_TICPP"}
    flags               {"ExtraWarnings"}
    links               {"plugin-interface", "TiCPP", "wxFlatNotebook"}

if wxVersion < "2.9" then
    defines             {"SCI_NAMESPACE", "__WX__"}
    links               {"wxPropertyGrid", "wxScintilla"}
else
    excludes
    {
        "../../../src/controls/include/wx/propgrid/**.h",
        "../../../src/controls/include/wx/wxScintilla/**.h",
        "../../../src/controls/src/propgrid/**.cpp",
        "../../../src/controls/src/wxScintilla/**.cpp"
    }
end
    configuration {"codelite", "not windows"}
        linkoptions     {"-Wl,-rpath,$$``ORIGIN"}

    configuration {"codeblocks", "not windows"}
        linkoptions     {"-Wl,-rpath,$``ORIGIN"}

    configuration "not windows"
        targetdir       "../../../output/lib/wxformbuilder"

    configuration "windows"
        targetprefix    "lib"
        targetdir       "../../../output/plugins/wxAdditions"

    configuration "Debug"
        targetsuffix    ( DebugSuffix )
        wx_config       { Debug="yes" }

    configuration "Release"
        buildoptions    {"-fno-strict-aliasing"}
        wx_config       {}
