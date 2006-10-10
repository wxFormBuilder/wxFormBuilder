project.name = "wxFormBuilder SDK"
project.configs = { "Debug", "Debug (Unicode)", "Release", "Release (Unicode)" }

project.libdir = "lib"

-- Add sdk projects here. (This needs to match the directory name not the package name.)
dopackage( "plugin_interface" )
dopackage( "tinyxml" )
