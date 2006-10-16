project.name = "wxFormBuilder 3rd-Party Controls"
project.configs = { "Debug", "Debug (Unicode)", "Release", "Release (Unicode)" }

project.libdir = "lib"
dofile( "../../../wrap_wxconfig.lua" )

wxCfgInit( ".."..wxCFG_PATH_SEP..".."..wxCFG_PATH_SEP..".."..wxCFG_PATH_SEP )
wxCfgGetWxVersion()

-- Add sdk projects here.
dopackage( "propgrid" )
--dopackage( "wxFlatNotebook" )
--dopackage( "wxScintilla" )
