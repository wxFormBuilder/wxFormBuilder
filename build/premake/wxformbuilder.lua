-----------------------------------------------------------------------------
--  Name:        wxformbuilder.lua
--  Purpose:     Main application project
--  Author:      Andrea Zanellato
--  Modified by: 
--  Created:     19/10/2011
--  Copyright:   (c) 2011 wxFormBuilder Team
--  Licence:     GNU General Public License Version 2
-----------------------------------------------------------------------------
project "wxFormBuilder"
    kind                    "WindowedApp"
    files
    {
        "../../src/**.h", "../../src/**.hpp", "../../src/**.hh",
        "../../src/**.cpp", "../../src/**.cc", "../../src/**.fbp"
    }
    excludes                { "../../src/controls/**" }
    includedirs
    {
        "../../src", "../../src/boost", "../../src/controls/include",
        "../../sdk/tinyxml", "../../sdk/plugin_interface"
    }
    defines                 {"NO_GCC_PRAGMA", "TIXML_USE_TICPP", "APPEND_WXVERSION"}
    flags                   {"ExtraWarnings"}
    libdirs                 {"../../sdk/lib"}
    links                   {"TiCPP", "plugin-interface"}

    local libs = ""
if wxVersion < "2.9" then
	defines					{"USE_FLATNOTEBOOK"}
    links                   {"wxPropertyGrid", "wxScintilla", "wxFlatNotebook"}
else
    excludes
    {
        "../../../src/controls/include/wx/propgrid/**.h",
        "../../../src/controls/include/wx/wxScintilla/**.h",
        "../../../src/controls/include/wx/wxFlatNotebook/**.h",
        "../../../src/controls/src/propgrid/**.cpp",
        "../../../src/controls/src/wxScintilla/**.cpp",
        "../../../src/controls/src/wxFlatNotebook/**.cpp"
    }
    libs                    = "all"
end
    configuration {"codelite", "not windows"}
        linkoptions         {"-Wl,-rpath,$$``ORIGIN/../lib/wxformbuilder"}

    configuration {"codeblocks", "not windows"}
        linkoptions         {"-Wl,-rpath,$``ORIGIN/../lib/wxformbuilder"}

    configuration "macosx"
        linkoptions         {"-L../../output/lib/wxformbuilder"}

    configuration {"macosx", "Debug"}
        postbuildcommands   {"../macosx/postbuildd.sh"}

    configuration {"macosx", "Release"}
        postbuildcommands   {"../macosx/postbuild.sh"}

    configuration "not windows"
        excludes            {"../../src/*.rc"}
        libdirs             {"../../output/lib/wxformbuilder"}
        targetdir           "../../output/bin"
        targetname          "wxformbuilder"

    configuration "windows"
        files               {"../../src/*.rc"}
        libdirs             {"../../output"}
        targetdir           "../../output"

if wxCompiler == "gcc" then
        buildoptions        {"-gstabs"}
        links               {"bfd", "intl", "iberty", "psapi", "imagehlp"}
end
    configuration "Debug"
        defines             {"__WXFB_DEBUG__"}
        targetsuffix        ( DebugSuffix )
        wx_config           { Libs=libs, Debug="yes" }

    configuration "Release"
        buildoptions        {"-fno-strict-aliasing"}
        wx_config           { Libs=libs }

    if os.is("windows") then
        flags               {"Symbols"}
    end
