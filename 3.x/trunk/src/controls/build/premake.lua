if ( OS == "windows" ) then
	project.name = "wxFormBuilder 3rd-Party Controls"
	project.configs = { "Debug", "Debug (Unicode)", "Release", "Release (Unicode)" }
else
	project.name = "wxFormBuilder 3rd-Party Controls-unix"
end
project.libdir = "lib"

-- Add sdk projects here.
dopackage( "propgrid" )
--dopackage( "wxFlatNotebook" )
--dopackage( "wxScintilla" )
