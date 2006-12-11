package.name = "wxFormBuilder"

package.kind = "winexe"
package.language = "c++"
package.files = { matchrecursive( "*.cpp", "*.h", "*.rc" ) }
package.excludes = { matchrecursive( "controls/*.cpp", "controls/*.h" ) }

-- Local variables
local cbSpecific = ""
local wx_ver = "27"

-- Set object output directory.
package.config["Debug"].objdir = ".objsd"
package.config["Debug (Unicode)"].objdir = ".objsud"
package.config["Release"].objdir = ".objs"
package.config["Release (Unicode)"].objdir = ".objsu"

-- Set the targets.
if ( OS == "windows") then
	package.config["Debug"].target = "wxFormBuilderd"
	package.config["Debug (Unicode)"].target = "wxFormBuilderd"
	package.config["Release"].target = "wxFormBuilder"
	package.config["Release (Unicode)"].target = "wxFormBuilder"
else
	package.config["Debug"].target = "wxFormBuilderd"
	package.config["Release"].target = "wxFormBuilder"
end

-- Set the build options for the Unicode build Targets.
package.buildflags = { "extra-warnings" }
package.config["Debug (Unicode)"].buildflags = { "unicode" }
package.config["Release"].buildflags = { "no-symbols", "optimize-speed" }
package.config["Release (Unicode)"].buildflags = { "unicode", "no-symbols", "optimize-speed" }

-- Set include paths
if ( target == "cb-gcc" ) then
	cbSpecific = "$(#WX)/include"
else
	cbSpecific = "$(WXWIN)/include"
end
package.includepaths =
{
	cbSpecific,
	"controls/include",
	"boost",
	"../src", 
	"../sdk/tinyxml",
	"../sdk/plugin_interface"
}

-- Setup the linker options.
if ( target == "cb-gcc" or target == "gnu" ) then
	if ( target == "cb-gcc" ) then
		cbSpecific = "$(#WX)/lib/gcc_dll"
	else
		cbSpecific = "$(WXWIN)/lib/gcc_dll"
	end
	package.libpaths =
	{
		cbSpecific,
		"controls/lib/gcc_dll",
		"../bin",
		"../sdk/lib"
	}
else
	if ( target == "cb-gcc" ) then
		cbSpecific = "$(#WX)/lib/vc_dll"
	else
		cbSpecific = "$(WXWIN)/lib/vc_dll"
	end
	package.libpaths =
	{
		cbSpecific,
		"controls/lib/vc_dll",
		"../sdk/lib"
	}
end

-- Setup the output directory options.
package.bindir = "../bin"
--package.libpath = "controls/lib/gcc_dll"

-- Set libraries to link.
if ( OS == "windows") then
	
	package.config["Debug"].links = { "wxmsw"..wx_ver.."d" }
	package.config["Debug (Unicode)"].links = { "wxmsw"..wx_ver.."ud" }
	package.config["Release"].links = {	"wxmsw"..wx_ver }
	package.config["Release (Unicode)"].links = { "wxmsw"..wx_ver.."u" }
	package.links = { "wxFlatNotebook", "wxPropGrid", "wxScintilla", "TiCPP", "Plugin Interface" }
else
	package.config["Debug"].linkoptions = { "`wx-config --debug --libs`"}
	package.config["Release"].linkoptions = { "`wx-config --libs`" }
end

-- Set defines.
if ( OS == "windows") then
	package.defines =
	{
		"HAVE_W32API_H",
		"WIN32",
		"_WINDOWS",
		"TIXML_USE_STL",
		"__WX__",
		"__WXMSW__",
		"WXUSINGDLL",
		"WXUSINGDLL_FNB",
		"TIXML_USE_TICPP"
	}
	package.config["Debug"].defines = { "DEBUG", "__WXDEBUG__" }
	package.config["Debug (Unicode)"].defines = { "DEBUG", "__WXDEBUG__", "UNICODE", "_UNICODE" }
	package.config["Release"].defines = { "NDEBUG" }
	package.config["Release (Unicode)"].defines = { "NDEBUG", "UNICODE", "_UNICODE" }
else
	package.defines =
	{
		"TIXML_USE_STL",
		"__WX__",
		"__WXGTK__",
		"NO_GCC_PRAGMA",
		"NOPCH",
		"WXUSINGDLL",
		"TIXML_USE_TICPP"
	}
	package.config["Debug"].defines = { "DEBUG", "__WXDEBUG__", "WXUSINGDLL" }
	package.config["Release"].defines = { "NDEBUG", "WXUSINGDLL" }
end

-- Set build options for Linux.
if ( OS == "linux" ) then
	package.config["Debug"].buildoptions = { "`wx-config --debug=yes --cflags`" }
	package.config["Release"].buildoptions = { "`wx-config --debug=no --cflags`" }
end

