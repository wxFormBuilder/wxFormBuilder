project.name = "wxFormBuilder"
if ( OS == "windows" ) then
	project.configs = { "Debug", "Debug (Unicode)", "Release", "Release (Unicode)" }
end

-- Add sdk projects here.
dopackage( "sdk/plugin_interface" )
dopackage( "sdk/tinyxml" )
