package.name = "Plugin Interface"

package.kind = "lib"
package.libdir = "../lib"
package.language = "c++"
package.files = { matchfiles( "*.h", "*.cpp" ) }

-- Set object output directory.
package.config["Debug"].objdir = ".objsd"
package.config["Debug (Unicode)"].objdir = ".objsud"
package.config["Release"].objdir = ".objs"
package.config["Release (Unicode)"].objdir = ".objsu"

-- Set the targets.
package.config["Debug"].target = "fbPluginInterfaced"
package.config["Debug (Unicode)"].target = "fbPluginInterfaceud"
package.config["Release"].target = "fbPluginInterface"
package.config["Release (Unicode)"].target = "fbPluginInterfaceu"

-- Set the build options for the Unicode build Targets.
package.buildflags = { "extra-warnings" }
package.config["Debug (Unicode)"].buildflags = { "unicode" }
package.config["Release"].buildflags = { "no-symbols", "optimize-speed" }
package.config["Release (Unicode)"].buildflags = { "unicode", "no-symbols", "optimize-speed" }

-- Set include paths
package.includepaths = { "$(WXWIN)/include", "../tinyxml" }

-- Set defines.
if ( OS == "windows") then
	package.config["Debug"].defines = { "DEBUG", "WIN32", "_WINDOWS", "HAVE_W32API_H", "__WX__", "__WXMSW__", "__WXDEBUG__", "TIXML_USE_TICPP", "WXUSINGDLL" }
	package.config["Debug (Unicode)"].defines = { "DEBUG", "WIN32", "_WINDOWS", "HAVE_W32API_H", "__WX__", "__WXMSW__", "__WXDEBUG__", "TIXML_USE_TICPP", "UNICODE", "_UNICODE", "WXUSINGDLL" }
	package.config["Release"].defines = { "NDEBUG", "WIN32", "_WINDOWS", "HAVE_W32API_H", "__WX__", "__WXMSW__", "TIXML_USE_TICPP", "WXUSINGDLL" }
	package.config["Release (Unicode)"].defines = { "NDEBUG", "WIN32", "_WINDOWS", "HAVE_W32API_H", "__WX__", "__WXMSW__", "TIXML_USE_TICPP", "UNICODE", "_UNICODE", "WXUSINGDLL" }
else
	package.config["Debug"].defines = { "DEBUG", "__WX__", "__WXDEBUG__", "TIXML_USE_TICPP", "WXUSINGDLL" }
	package.config["Release"].defines = { "NDEBUG", "__WX__", "TIXML_USE_TICPP", "WXUSINGDLL" }
end

-- Set build optionsfor Linux.
if ( OS == "linux" ) then
	package.config["Debug"].buildoptions = { "`wx-config --debug=yes --cflags`" }
	package.config["Release"].buildoptions = { "`wx-config --debug=no --cflags`" }
end

