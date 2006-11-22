project.name = "wxFormBuilder"
if ( OS == "windows" ) then
	project.configs = { "Debug", "Debug (Unicode)", "Release", "Release (Unicode)" }
end

--project.path = "../"

-- Add packages here.
dopackage( "src" )
dopackage( "sdk/plugin_interface" )
dopackage( "sdk/tinyxml" )
dopackage( "src/controls/build/propgrid" )
dopackage( "src/controls/build/wxFlatNotebook" )
dopackage( "src/controls/build/wxScintilla" )
