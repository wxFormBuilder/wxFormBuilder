-----------------------------------------------------------------------------
--  Name:        solution.lua
--  Purpose:     Generic Premake 4 solution defining common configurations
--               for all projects it contains.
--  Author:      Andrea Zanellato
--  Modified by: 
--  Created:     19/10/2011
--  Copyright:   (c) 2011 wxFormBuilder Team
--  Licence:     GNU General Public License Version 2
-----------------------------------------------------------------------------
solution "wxFormBuilder-Solution"
    language "C++"
    configurations      {"Debug", "Release"}

    local scriptDir     = os.getcwd()

    dofile( scriptDir .. "/wxwidgets.lua" )

    local wxver         = string.gsub( wxVersion, '%.', '' )
    location            ( "../../build/" .. wxVersion .. "/" .. _ACTION )
    BuildDir            = solution().location
	CustomPrefix        = "wx_" .. wxTarget .. wxUnicodeSign

if wxVersion < "2.9" then
    DebugSuffix         = "d-" .. wxver
else
    DebugSuffix         = "-" .. wxver
end
    os.chdir( BuildDir )

--if wxCompiler == "gcc" and os.is("windows") then
--  flags               {"NoImportLib"}
--end

if wxUseUnicode then
    flags               {"Unicode"}
    defines             {"UNICODE", "_UNICODE"}
end
    configuration "windows"
        defines         {"WIN32", "_WINDOWS"}

    configuration "Debug"
        defines         {"DEBUG", "_DEBUG"}
        flags           {"Symbols"}

		if wxCompiler == "gcc" then
			buildoptions    {"-O0"}
		end

    configuration "Release"
		if wxCompiler == "gcc" then
			linkoptions {"-s"}
		end
        defines         {"NDEBUG"}
        flags           {"OptimizeSpeed"}

    dofile( scriptDir .. "/ticpp.lua" )
    dofile( scriptDir .. "/plugin-interface.lua" )
	dofile( scriptDir .. "/wxflatnotebook.lua" )

if wxVersion < "2.9" then
    dofile( scriptDir .. "/wxpropgrid.lua" )
    dofile( scriptDir .. "/wxscintilla.lua" )
end
    dofile( scriptDir .. "/plugins/additional.lua" )
    dofile( scriptDir .. "/plugins/common.lua" )
    dofile( scriptDir .. "/plugins/containers.lua" )
    dofile( scriptDir .. "/plugins/forms.lua" )
    dofile( scriptDir .. "/plugins/layout.lua" )
    dofile( scriptDir .. "/plugins/wxadditions-mini.lua" )
    dofile( scriptDir .. "/wxformbuilder.lua" )
    dofile( scriptDir .. "/utilities.lua" )

