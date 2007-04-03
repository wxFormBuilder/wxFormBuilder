project.name = "wxFormBuilder"

-- Add packages here.
dopackage( "src" )
dopackage( "plugins/additional" )
dopackage( "plugins/common" )
dopackage( "plugins/containers" )
dopackage( "plugins/layout" )
dopackage( "plugins/wxAdditions" )
dopackage( "sdk/plugin_interface" )
dopackage( "sdk/tinyxml" )
dopackage( "src/controls/build/propgrid" )
dopackage( "src/controls/build/wxFlatNotebook" )
dopackage( "src/controls/build/wxScintilla" )

-- Pre-build file creation steps
function CreateShareReadme( filename )
	local text = 
	"This directory and the symlink it contains were created automatically by premake to\n" ..
	"facilitate execution of wxFormBuilder prior to installation on Unix platforms. On Unix,\n" ..
	"wxFormBuilder expects to be executed from a directory named \"output\", which is next to\n" ..
	" a directory named \"share\". The \"share\" directory should have a subdirectory named\n" ..
	"\"wxformbuilder\", which contains the configuration files. "
	
	if ( not os.fileexists( filename ) ) then
		local fo = io.open( filename, "w" )
		fo:write( text )
		fo:close()
	end
end

function CreateSymlink( symLink, pathToLink )
	os.execute( "ln -s " .. pathToLink .. " "..symLink  )
end

if ( not windows ) then
	print( "Running pre-build file creation steps..." )
	-- Create a 'share' directory.
	os.execute( "mkdir output/share" )
	CreateShareReadme( "output/share/README.txt" )
	CreateSymlink( "../output", "output/share/wxformbuilder" )
end
