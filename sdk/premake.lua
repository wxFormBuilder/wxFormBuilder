project.name = "wxFormBuilder SDK"

-- Add sdk projects here. (This needs to match the directory name not the package name.)
dopackage( "plugin_interface" )
dopackage( "tinyxml" )

-- Add options here.
addoption( "dynamic-runtime", "Use the dynamicly loadable version of the runtime." )
addoption( "unicode", "Use the Unicode character set" )

