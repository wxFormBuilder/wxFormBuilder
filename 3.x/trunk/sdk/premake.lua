if ( OS == "windows" ) then
	project.name = "wxFormBuilder SDK"
	project.configs = { "Debug", "Debug (Unicode)", "Release", "Release (Unicode)" }
else
	project.name = "wxFormBuilder SDK-unix"
end
project.libdir = "lib"

-- Add sdk projects here.
dopackage( "plugin_interface" )
dopackage( "tinyxml" )
