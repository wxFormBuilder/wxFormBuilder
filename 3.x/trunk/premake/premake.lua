project.name = "wxFormBuilder"
if ( OS == "windows" ) then
	project.configs = { "Debug", "Debug (Unicode)", "Release", "Release (Unicode)" }
end

project.path = "../"

-- Add sdk projects here.
dopackage( "../sdk/plugin_interface" )
dopackage( "../sdk/tinyxml" )
