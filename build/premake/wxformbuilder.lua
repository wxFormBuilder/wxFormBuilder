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
        "../../src",
        "../../sdk/tinyxml", "../../sdk/plugin_interface"
    }
    defines                 {"NO_GCC_PRAGMA", "TIXML_USE_TICPP", "APPEND_WXVERSION"}
    libdirs                 {"../../sdk/lib"}
    links                   {"TiCPP", "plugin-interface"}

    local libs = ""
	if wxUseMediaCtrl then
		libs	= "std,stc,richtext,propgrid,aui,ribbon,media"
	else
		libs	= "std,stc,richtext,propgrid,aui,ribbon"
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

    configuration "not vs*"
        buildoptions        "-std=c++14"

    configuration "vs*"
        defines         {"_CRT_SECURE_NO_DEPRECATE", "_CRT_SECURE_NO_WARNINGS"}

    configuration "macosx"
        linkoptions         {"-Wl,-L../../../output/lib/wxformbuilder"}

    configuration {"macosx", "Debug"}
        postbuildcommands   {"sh ../../../install/macosx/postbuild.sh -c debug" }

    configuration {"macosx", "Release"}
        postbuildcommands   {"sh ../../../install/macosx/postbuild.sh -c release" }

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
        wx_config           { Libs=libs }

    configuration {"not vs*", "Release"}
        buildoptions    {"-fno-strict-aliasing"}
