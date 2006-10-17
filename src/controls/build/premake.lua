project.name = "wxFormBuilder 3rd-Party Controls"
project.configs = { "Debug", "Debug (Unicode)", "Release", "Release (Unicode)" }

project.libdir = "lib"
dofile( "../../../premake/wrap_wxconfig.lua" )
wxconfigPath = ".." .. wxCFG_PATH_SEP .. ".." .. wxCFG_PATH_SEP .. ".." .. wxCFG_PATH_SEP .. "premake" .. wxCFG_PATH_SEP
wxCfgInit( wxconfigPath )

--Test out functionality
print( "wxWidgets version = " .. wxCfgGetWxVersion() )
print( "wxWidgets Libs = " .. wxCfgGetLibs() )
print( "wxWidgets cflags = " .. wxCfgGetCFlags() )

-- Add sdk projects here.
dopackage( "propgrid" )
--dopackage( "wxFlatNotebook" )
--dopackage( "wxScintilla" )
