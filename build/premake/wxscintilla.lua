-----------------------------------------------------------------------------
--  Name:        wxscintilla.lua
--  Purpose:     wxScintilla library build script for wxWidgets 2.8.
--  Author:      Andrea Zanellato
--  Modified by: 
--  Created:     19/10/2011
--  Copyright:   (c) 2011 wxFormBuilder Team
--  Licence:     GNU General Public License Version 2
-----------------------------------------------------------------------------
project "wxScintilla"
    kind                "SharedLib"
    files
    {
        "../../src/controls/src/wxScintilla/*.cpp",
        "../../src/controls/include/wx/wxScintilla/*.h",
        "../../src/controls/src/wxScintilla/scintilla/**.cxx",
        "../../src/controls/src/wxScintilla/scintilla/**.h"
    }
    includedirs
    {
        "../../src/controls/include", "../../src/controls/src/wxScintilla",
        "../../src/controls/src/wxScintilla/scintilla/include",
        "../../src/controls/src/wxScintilla/scintilla/src"
    }
    defines
    {
        "WXMAKINGDLL_SCI","LINK_LEXERS","SCI_LEXER","SCI_NAMESPACE","__WX__"
    }
    targetsuffix        ( "-" .. wxVersion .. "_wxfb" )
	
	if wxArchitecture then
		buildoptions	{"-arch " .. wxArchitecture}
	end

    configuration "linux or bsd"
        defines         {"GTK"}

    configuration "not windows"
        targetdir       "../../output/lib/wxformbuilder"

    configuration "windows"
        links           {"Gdi32"}
        targetdir       "../../output"

    configuration "Debug"
        targetname      ( CustomPrefix .. wxDebugSuffix .. "_scintilla" )
        wx_config       { Debug="yes" }

    configuration "Release"
        buildoptions    {"-fno-strict-aliasing"}
        targetname      ( CustomPrefix .. "_scintilla" )
        wx_config       {}
