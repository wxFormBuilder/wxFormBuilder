project.name = "wxFormBuilder"
if ( windows ) then
	project.bindir = "output"
else
	project.bindir = "output/lib/wxformbuilder"
end

-- Configureations. All I am doing is reordering them so that
-- it defaults to a Release build.
project.configs = { "Release", "Debug" }

-- Add packages here.
dopackage( "src/controls/build/propgrid" )
dopackage( "src/controls/build/wxFlatNotebook" )
dopackage( "src/controls/build/wxScintilla" )
dopackage( "sdk/plugin_interface" )
dopackage( "sdk/tinyxml/ticpp.lua" )
dopackage( "plugins/additional" )
dopackage( "plugins/common" )
dopackage( "plugins/containers" )
dopackage( "plugins/layout" )
dopackage( "plugins/wxAdditions" )

-- do this package last, so the post build step actually happens at the end of all the building
dopackage( "src" )

-- Pre-build file creation steps
function CreateShareReadme( filename )
	local text = 
	"This directory and the symlink it contains were created automatically by premake\n" ..
	"to facilitate execution of wxFormBuilder prior to installation on Unix platforms.\n" ..
	"On Unix, wxFormBuilder expects to be executed from a directory named \"output\",\n" ..
	"which is next to a directory named \"share\". The \"share\" directory should have a\n" ..
	"subdirectory named \"wxformbuilder\", which contains the configuration files."
	
	if ( not os.fileexists( filename )  ) then
		local fo = io.open( filename, "w" )
		fo:write( text )
		fo:close()
	end
end

function CreateSymlink( pathToLink, symLink )
	os.execute( "ln -s -f -n " .. pathToLink .. " " .. symLink  )
end

if ( not windows ) then
	addoption( "skip-symlink", "Do not create the symlink output/share/wxformbuilder" )
	if ( not options["skip-symlink"] ) then
		print( "Running pre-build file creation steps..." )
		-- Create a 'share' directory.
		-- HACK: make sure the warning isn't shown on the console.
		os.execute( "mkdir output/share >/dev/null 2>/dev/null" )
		CreateShareReadme( "output/share/README.txt" )
		CreateSymlink( "../../output", "output/share/wxformbuilder" )
	end
end

if ( linux ) then
addoption("install", "Copy the application to the specified path")
	
function doinstall(cmd, arg)
	if (not arg) then
		error("You must specify an install location")
	end
		
	os.execute( "install/linux/wxfb_export.sh " .. arg )
end

end
