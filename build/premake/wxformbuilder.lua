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
    excludes
	{
		"../../src/controls/**",
		"../../src/rad/designer/resizablepanel.*"
	}
    includedirs
    {
        "../../src", "../../src/boost",
        "../../sdk/tinyxml", "../../sdk/plugin_interface"
    }
if wxVersion < "2.9" then
    includedirs
    {
        "../../src/controls/include",
    }
end
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
        "../../src/controls/include/wx/propgrid/**.h",
        "../../src/controls/include/wx/wxScintilla/**.h",
        "../../src/controls/include/wx/wxFlatNotebook/**.h",
        "../../src/controls/src/propgrid/**.cpp",
        "../../src/controls/src/wxScintilla/**.cpp",
        "../../src/controls/src/wxFlatNotebook/**.cpp"
    }
	if wxUseMediaCtrl then
		libs	= "std,stc,richtext,propgrid,aui,ribbon,media"
	else
		libs	= "std,stc,richtext,propgrid,aui,ribbon"
	end
end

	if wxArchitecture then
		buildoptions	{"-arch " .. wxArchitecture}
	end
	
	if os.is( "linux" ) then
		newoption
		{
			trigger		= "rpath",
			description	= "Linux only, set rpath on the linker line to find shared libraries next to executable"
		}

		-- Set rpath
		local useRpath = true
		local rpath= _ACTION == "codeblocks" and "$" or "$$"
		rpath = rpath .. "``ORIGIN/../lib/wxformbuilder"
		local rpathOption = _OPTIONS[ "rpath" ]

		if rpathOption then
			if "no" == rpathOption or "" == rpathOption then
				useRpath = false
			else
				rpath = rpathOption
			end
		end

		if useRpath then
		print( "rpath: -Wl,-rpath," .. rpath )
			linkoptions( "-Wl,-rpath," .. rpath )
		end
	end
	
    configuration "macosx"
        linkoptions         {"-Wl,-L../../../output/lib/wxformbuilder"}

    configuration {"macosx", "Debug"}
        postbuildcommands   {"../../../install/macosx/postbuildd4.sh"}

    configuration {"macosx", "Release"}
        postbuildcommands   {"../../../install/macosx/postbuild4.sh"}

    configuration "not windows"
        excludes            {"../../src/*.rc"}
        libdirs             {"../../output/lib/wxformbuilder"}
        targetdir           "../../output/bin"
        targetname          "wxformbuilder"
        links               {"dl"}

    configuration "windows"
        files               {"../../src/*.rc"}
        libdirs             {"../../output"}
        targetdir           "../../output"
		flags               {"Symbols", "WinMain"}

if wxCompiler == "gcc" then
        buildoptions        {"-gstabs"}
        links               {"bfd", "iberty", "psapi", "imagehlp"}
		if not ( wxCompilerName == "mingw64" ) then
			links               {"intl"}
		end
end
    configuration "Debug"
        defines             {"__WXFB_DEBUG__"}
        targetsuffix        ( DebugSuffix )
        wx_config           { Libs=libs, Debug="yes" }

    configuration "Release"
        buildoptions        {"-fno-strict-aliasing"}
        wx_config           { Libs=libs }
