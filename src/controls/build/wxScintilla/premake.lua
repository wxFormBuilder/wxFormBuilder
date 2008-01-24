--*****************************************************************************
--*	Author:		RJP Computing <rjpcomputing@gmail.com>
--*	Date:		12/15/2006
--*	Version:	1.00-beta
--*	
--*	NOTES:
--*		- use the '/' slash for all paths.
--*****************************************************************************

function trim (s)
    return (string.gsub(s, "^%s*(.-)%s*$", "%1"))
end

-- wxWidgets version
local wx_ver = "28"
local wx_ver_minor = ""
local wx_custom = "_wxfb"

--******* Initial Setup ************
--*	Most of the setting are set here.
--**********************************

-- Set the name of your package.
package.name = "wxScintilla"
-- Set this if you want a different name for your target than the package's name.
local targetName = "scintilla"
-- Set the kind of package you want to create.
--		Options: exe | winexe | lib | dll
package.kind = "dll"
-- Set the files to include.
package.files =
{
	matchfiles( "../../src/wxScintilla/*.cpp", "../../include/wx/wxScintilla/*.h" ),
	matchrecursive( "../../src/wxScintilla/scintilla/*.cxx", "../../src/wxScintilla/scintilla/*.h" )
}
-- Set the include paths.
package.includepaths = { "../../include", "../../src/wxScintilla", "../../src/wxScintilla/scintilla/include", "../../src/wxScintilla/scintilla/src" }
-- Set libraries to link.
if ( windows ) then
	package.links = { "Gdi32" }
end

-- Set the defines.
package.defines = { "WXMAKINGDLL_SCI", "MONOLITHIC", "LINK_LEXERS", "SCI_LEXER", "SCI_NAMESPACE", "__WX__" }

--------------------------- DO NOT EDIT BELOW ----------------------------------

--******* GENAERAL SETUP **********
--*	Settings that are not dependant
--*	on the operating system.
--*********************************
-- Package options
addoption( "unicode", "Use the Unicode character set" )
addoption( "with-wx-shared", "Link against wxWidgets as a shared library" )
if ( not windows ) then
	addoption( "disable-wx-debug", "Compile against a wxWidgets library without debugging" )
end

-- Common setup
package.language = "c++"

-- Set object output directory.
if ( options["unicode"] ) then
	package.config["Debug"].objdir = ".objsud"
	package.config["Release"].objdir = ".objsu"
else
	package.config["Debug"].objdir = ".objsd"
	package.config["Release"].objdir = ".objs"
end

-- Set debug flags
if ( options["disable-wx-debug"] and ( not windows ) ) then
	debug_option = "--debug=no"
	debug_macro = { "NDEBUG", "__WXFB_DEBUG__" }
else
	debug_option = "--debug=yes"
	debug_macro = { "DEBUG", "_DEBUG", "__WXDEBUG__" }
end

-- Set the default targetName if none is specified.
if ( string.len( targetName ) == 0 ) then
	targetName = package.name
end

-- Set the build options.
package.config["Release"].buildflags = { "no-symbols", "optimize-speed" }
if ( options["unicode"] ) then
	table.insert( package.buildflags, "unicode" )
end
if ( target == "cb-gcc" or target == "gnu" ) then
	table.insert( package.buildflags, "no-import-lib" )
	table.insert( package.config["Debug"].buildoptions, "-O0" )
	table.insert( package.config["Release"].buildoptions, "-fno-strict-aliasing" )
end

-- Set the defines.
if ( options["with-wx-shared"] ) then
	table.insert( package.defines, "WXUSINGDLL" )
end
if ( options["unicode"] ) then
	table.insert( package.defines, { "UNICODE", "_UNICODE" } )
end

table.insert( package.config["Debug"].defines, debug_macro )
table.insert( package.config["Release"].defines, "NDEBUG" )

if ( windows ) then
--******* WINDOWS SETUP ***********
--*	Settings that are Windows specific.
--*********************************
	-- Set wxWidgets include paths 
	if ( target == "cb-gcc" ) then
		table.insert( package.includepaths, "$(#WX.include)" )
	else
		table.insert( package.includepaths, "$(WXWIN)/include" )
	end
	
	-- Set the correct 'setup.h' include path.
	if ( options["with-wx-shared"] ) then
		if ( options["unicode"] ) then
			if ( target == "cb-gcc" ) then
				table.insert( package.config["Debug"].includepaths, "$(#WX.lib)/gcc_dll/mswud" )
				table.insert( package.config["Release"].includepaths, "$(#WX.lib)/gcc_dll/mswu" )
			elseif ( target == "gnu" ) then
				table.insert( package.config["Debug"].includepaths, "$(WXWIN)/lib/gcc_dll/mswud" )
				table.insert( package.config["Release"].includepaths, "$(WXWIN)/lib/gcc_dll/mswu" )
			else
				table.insert( package.config["Debug"].includepaths, "$(WXWIN)/lib/vc_dll/mswud" )
				table.insert( package.config["Release"].includepaths, "$(WXWIN)/lib/vc_dll/mswu" )
			end
		else
			if ( target == "cb-gcc" ) then
				table.insert( package.config["Debug"].includepaths, "$(#WX.lib)/gcc_dll/mswd" )
				table.insert( package.config["Release"].includepaths, "$(#WX.lib)/gcc_dll/msw" )
			elseif ( target == "gnu" ) then
				table.insert( package.config["Debug"].includepaths, "$(WXWIN)/lib/gcc_dll/mswd" )
				table.insert( package.config["Release"].includepaths, "$(WXWIN)/lib/gcc_dll/msw" )
			else
				table.insert( package.config["Debug"].includepaths, "$(WXWIN)/lib/vc_dll/mswd" )
				table.insert( package.config["Release"].includepaths, "$(WXWIN)/lib/vc_dll/msw" )
			end
		end
	else
		if ( options["unicode"] ) then
			if ( target == "cb-gcc" ) then
				table.insert( package.config["Debug"].includepaths, "$(#WX.lib)/gcc_lib/mswud" )
				table.insert( package.config["Release"].includepaths, "$(#WX.lib)/gcc_lib/mswu" )
			elseif ( target == "gnu" ) then
				table.insert( package.config["Debug"].includepaths, "$(WXWIN)/lib/gcc_lib/mswud" )
				table.insert( package.config["Release"].includepaths, "$(WXWIN)/lib/gcc_lib/mswu" )
			else
				table.insert( package.config["Debug"].includepaths, "$(WXWIN)/lib/vc_lib/mswud" )
				table.insert( package.config["Release"].includepaths, "$(WXWIN)/lib/vc_lib/mswu" )
			end
		else
			if ( target == "cb-gcc" ) then
				table.insert( package.config["Debug"].includepaths, "$(#WX.lib)/gcc_lib/mswd" )
				table.insert( package.config["Release"].includepaths, "$(#WX.lib)/gcc_lib/msw" )
			elseif ( target == "gnu" ) then
				table.insert( package.config["Debug"].includepaths, "$(WXWIN)/lib/gcc_lib/mswd" )
				table.insert( package.config["Release"].includepaths, "$(WXWIN)/lib/gcc_lib/msw" )
			else
				table.insert( package.config["Debug"].includepaths, "$(WXWIN)/lib/vc_lib/mswd" )
				table.insert( package.config["Release"].includepaths, "$(WXWIN)/lib/vc_lib/msw" )
			end
		end
	end
	
	-- Set the linker options.
	if ( options["with-wx-shared"] ) then
		if ( target == "cb-gcc" ) then
			table.insert( package.libpaths, "$(#WX.lib)/gcc_dll" )
		elseif ( target == "gnu" ) then
			table.insert( package.libpaths, "$(WXWIN)/lib/gcc_dll" )
		else
			table.insert( package.libpaths, "$(WXWIN)/lib/vc_dll" )
		end
	else
		if ( target == "cb-gcc" ) then
			table.insert( package.libpaths, "$(#WX.lib)/gcc_lib" )
		elseif ( target == "gnu" ) then
			table.insert( package.libpaths, "$(WXWIN)/lib/gcc_lib" )
		else
			table.insert( package.libpaths, "$(WXWIN)/lib/vc_lib" )
		end
	end
	
	-- Set wxWidgets libraries to link.
	if ( options["unicode"] ) then
		table.insert( package.config["Release"].links, "wxmsw"..wx_ver.."u" )
		table.insert( package.config["Debug"].links, "wxmsw"..wx_ver.."ud" )
	else
		table.insert( package.config["Release"].links, "wxmsw"..wx_ver )
		table.insert( package.config["Debug"].links, "wxmsw"..wx_ver.."d" )
	end
	
	-- Set the Windows defines.
	table.insert( package.defines, { "__WXMSW__", "WIN32", "_WINDOWS" } )
	
	-- Set the targets.
	if ( target == "cb-gcc" or target == "gnu" ) then
		if ( options["unicode"] ) then
			package.config["Debug"].target = "wxmsw"..wx_ver..wx_ver_minor.."umd_"..targetName.."_gcc"..wx_custom
			package.config["Release"].target = "wxmsw"..wx_ver..wx_ver_minor.."um_"..targetName.."_gcc"..wx_custom
		else
			package.config["Debug"].target = "wxmsw"..wx_ver..wx_ver_minor.."md_"..targetName.."_gcc"..wx_custom
			package.config["Release"].target = "wxmsw"..wx_ver..wx_ver_minor.."m_"..targetName.."_gcc"..wx_custom
		end
	else
		if ( options["unicode"] ) then
			package.config["Debug"].target = "wxmsw"..wx_ver..wx_ver_minor.."umd_"..targetName.."_vc"..wx_custom
			package.config["Release"].target = "wxmsw"..wx_ver..wx_ver_minor.."um_"..targetName.."_vc"..wx_custom
		else
			package.config["Debug"].target = "wxmsw"..wx_ver..wx_ver_minor.."md_"..targetName.."_vc"..wx_custom
			package.config["Release"].target = "wxmsw"..wx_ver..wx_ver_minor.."m_"..targetName.."_vc"..wx_custom
		end
	end
else
--******* LINUX/MAC SETUP *************
--*	Settings that are Linux/Mac specific.
--*************************************
	-- Ignore resource files in Linux/Mac.
	table.insert( package.excludes, matchrecursive( "*.rc" ) )
	table.insert( package.buildoptions, "-fPIC" )
	
	-- Set wxWidgets build options.
	table.insert( package.config["Debug"].buildoptions, "`wx-config "..debug_option.." --cflags`" )
	table.insert( package.config["Release"].buildoptions, "`wx-config --debug=no --cflags`" )
	
	-- Set the wxWidgets link options.
	table.insert( package.config["Debug"].linkoptions, "`wx-config "..debug_option.." --libs`" )
	table.insert( package.config["Release"].linkoptions, "`wx-config --libs`" )
	
	-- Set the Linux defines.
	if ( linux ) then
		table.insert( package.defines, { "GTK" } )
	end
	
	-- Add buildflag for proper dll building.
	if ( macosx ) then
		table.insert( package.buildflags, "dylib" )
	end
	
	-- Get wxWidgets lib names
	local wxconfig = io.popen("wx-config " .. debug_option .. " --basename")
	local debugBasename = trim( wxconfig:read("*a") )
	wxconfig:close()
	
	wxconfig = io.popen("wx-config --debug=no --basename")
	local basename = trim( wxconfig:read("*a") )
	wxconfig:close()
	
	wxconfig = io.popen("wx-config --release")
	local release = trim( wxconfig:read("*a") )
	wxconfig:close()
	
	-- Set the targets.
	package.config["Debug"].target = debugBasename .. "_" .. targetName .. "-" .. release .. wx_custom
	package.config["Release"].target = basename .. "_" .. targetName .. "-" .. release .. wx_custom
end
