-----------------------------------------------------------------------------
--  Name:        wxwidgets.lua
--  Purpose:     wxWidgets configuration file for Premake4
--  Author:      laurent.humbertclaude@gmail.com, v.krishnakumar@gmail.com
--  Modified by: Andrea Zanellato zanellato.andrea@gmail.com
-----------------------------------------------------------------------------
-- Optional root folder of wxWidgets installation folder
newoption  {
    trigger     =   "wx-root",
    value       =   "PATH",
    description = {
                    "Path to wxwidgets root folder, by default, WXWIN envvar",
                    "will be used or wx-config found in path on POSIX"
    }
}
newoption  {
    trigger     =   "compiler",
    description =   "Compiler in use"
}
newoption  {
    trigger     =   "compiler-version",
    description =   "Compiler in use version (eg. 48 for GCC 4.8 or 120 for VC2013)"
}
newoption  {
    trigger     =   "disable-mediactrl",
    description =   "Whether to use wxMediaCtrl in wxMedia library"
}
newoption  {
    trigger     =   "disable-unicode",
    description =   "Whether to use Unicode or ANSI build (wxWidgets 2.8 only)"
}
newoption  {
    trigger     =   "disable-monolithic",
    description =   "Whether to use wxWidgets as monolithic DLL (MSW only)"
}
newoption  {
    trigger     =   "disable-shared",
    description =   "Whether to use wxWidgets as static build"
}
newoption  {
    trigger     =   "wx-version",
    description =   "wxWidgets version to use"
}
newoption  {
    trigger     =   "architecture",
    description =   "Targeted architecture (e.g i386)"
}
newoption  {
    trigger     =   "force-wx-config",
    description =   "Use wx-config for configuration even under MS Windows"
}
-- Common globals
wxCompiler        = _OPTIONS["compiler"]
wxCompilerName    = nil
wxCompilerVersion = _OPTIONS["compiler-version"] or ""
wxArchitecture    = _OPTIONS["architecture"]
wxDebugSuffix     = "d"
wxPrefix          = "wx_"
wxTarget          = "gtk2"
wxVersion         = _OPTIONS["wx-version"]
wxUnicodeSign     = "u"
wxUseMediaCtrl    = true
wxUseUnicode      = true
wxMonolithic      = true

if not wxCompiler then wxCompiler = "gcc" end
wxCompilerName = wxCompiler

if wxCompiler == "mingw64" then
	if not ( "x86_64" == wxArchitecture ) then
		buildoptions( "-m32" )
		linkoptions( "-m32" )
		resoptions( "-F pe-i386" )
	end
	wxCompiler = "gcc"
end

if os.is("windows") then
    wxPrefix = "wx"
    wxTarget = "msw"
elseif  os.is("macosx")  then
    wxTarget = "mac"
end
-----------------------------------------------------------------------------
-- The wx_config the parameters are.
--          Root    : path to wx root folder. Can be left empty if WXWIN is defined
--                    or if wx-config is accessible.
--          Debug   : "yes" use debug version of wxwidgets. Default to "no"
--          Version : one of '2.4', '2.5', '2.6', '2.7', '2.8', '2.9'. Default to '2.9'
--          Static  : indicates how wx is to be linked. Values are
--                      either "yes" for static linking or "no" for shared linking, Default to "no"
--          Unicode : use "yes" for unicode or "no" for ansi version.
--                      ansi version only available up to 2.8
--                      Default to "yes"
--          Universal : use universal configuration. Default to "no"
--          Libs    : a list of wx libraries that you want to link with.
--                      eg: "aui,media,html"
--                      Default to ""; base is implicit
--          WindowsCompiler : compiler used to compile windows libraries ( "vc" or "gcc" )
-----------------------------------------------------------------------------
function wx_config(options)

-- Parameter Checks
    local wrongParam        = false
    local allowedWxOptions  =
    {
        "Root", "Debug", "Host", "Version", "Static",
        "Unicode", "Universal", "Libs", "WindowsCompiler", "WithoutLibs"
    }
    for option in pairs(options) do
        if not table.contains(allowedWxOptions, option) then
            print ("unrecognized option '"..option.. "'")
            wrongParam = true
        end
    end
    if wrongParam then
        print("valid options are : '" .. table.concat(allowedWxOptions, "', '").."'")
    end

-- wxMediaCtrl
    if _OPTIONS["disable-mediactrl"] then
        wxUseMediaCtrl = false
    end

-- Use Monolithic
    if _OPTIONS["disable-monolithic"] then
        wxMonolithic = false
    end

-- Unicode setup
    local useUnicode = "yes"
    if _OPTIONS["disable-unicode"] and wxVersion < "2.9" then
        useUnicode = "no"
    end

-- Unicode static build
    local useStatic = "no"
    if _OPTIONS["disable-shared"] then
        useStatic = "yes"
    end

-- Use wx-config
	local useWXConfig = "no"
    if _OPTIONS["force-wx-config"] then
        useWXConfig = "yes"
    end

    wx_config_Private( options.Root             or "",
							options.Debug				or "",
							options.Host				or "",
							options.Version			or wxVersion,
							options.Static			or useStatic,
							options.Unicode			or useUnicode,
							options.Universal			or "",
							options.Libs				or "",
							options.WindowsCompiler	or wxCompiler,
							options.CompilerVersion	or wxCompilerVersion,
							options.WithoutLibs		or "no",
							options.UseWXConfig		or useWXConfig
                     )
end

function wx_config_Private(wxRoot, wxDebug, wxHost, wxVersion, wxStatic, wxUnicode, wxUniversal, wxLibs, wxCompiler, wxCompilerVersion, wxWithoutLibs, wxUseWXConfig)
    -- some options are not allowed for newer version of wxWidgets
    if wxVersion > "2.8" then -- alphabetical comparison may fail...
        wxDebugSuffix   = ""
        wxUnicode       = "yes"
    end

    -- the environment variable WXWIN override wxRoot parameter
    if os.getenv('WXWIN') then wxRoot = os.getenv('WXWIN') end

    --wx-root=PATH override wxRoot parameter and WXWIN one
    --( we need to force the wx-config path in case more than a configuration
    --  is present in the system e.g. wx 2.8 installed and we need to use a self built 2.9 one )
    if _OPTIONS["wx-root"] then
    -- print ("seen option '--wx-root=" .. _OPTIONS["wx-root"] .. "' overriding default root = '" .. wxRoot .. "'")
        wxRoot = _OPTIONS["wx-root"]
    end

    if wxUnicode ~= "yes" then
        wxUseUnicode    = false
        wxUnicodeSign   = ""
    end

    if wxDebug == "yes" then
        defines         {"__WXDEBUG__"}
    else
        flags           {"Optimize"}
    end

    if wxStatic == "yes" then
        flags { "StaticRuntime" }
    else
        defines { "WXUSINGDLL" }
    end

    -- function to compensate lack of wx-config program on windows
    -- but wait, look at http://sites.google.com/site/wxconfig/ for one !
    function wx_config_for_windows(wxCompiler, wxCompilerVersion)
        local wxBuildType   = ""  -- buildtype is one of "", "u", "d" or "ud"

        if wxUnicode ~= "" then wxBuildType = wxBuildType .. "u" end
        if wxDebug == "yes" then
            wxBuildType = wxBuildType .. "d"
        end

        local wxLibPath = path.join(wxRoot, "lib")
        wxLibPath = path.join(wxLibPath, wxCompiler .. wxCompilerVersion .. "_" .. iif(wxStatic == 'yes', 'lib', 'dll'))
        -- common defines
        defines{ "__WXMSW__" }

        -- common include path
        includedirs {
            path.join(wxLibPath, "msw" .. wxBuildType),   -- something like "%WXWIN%\lib\vc_lib\mswud" to find "wx/setup.h"
            path.join(wxRoot, "include")
            }

		if wxWithoutLibs == "no" then
			-- common library path
			libdirs { wxLibPath }

			-- add the libs
			libVersion = string.gsub(wxVersion, '%.', '') -- remove dot from version
			if wxMonolithic then
				links { "wxmsw"..libVersion..wxBuildType }
			else
				links { "wxbase"..libVersion..wxBuildType } -- base lib
				local libs = "net,xml,adv,core,html,gl,aui,propgrid,ribbon,richtext,stc,webview,xrc"
				if wxUseMediaCtrl then libs = libs .. ",media" end
				for i, lib in ipairs(string.explode(libs, ",")) do
					local libPrefix = 'wxmsw'
					if lib == "xml" or lib == "net" or lib == "odbc" then
						libPrefix = 'wxbase'
					end
					links { libPrefix..libVersion..wxBuildType..'_'..lib}
				end
				-- link with support libraries
				for i, lib in ipairs({"wxjpeg", "wxpng", "wxzlib", "wxtiff", "wxexpat"}) do
					links { lib..wxDebugSuffix }
				end
				links { "wxregex" .. wxBuildType }
			end

			if string.match(_ACTION, "vs(.*)$") then
				-- link with MSVC support libraries
				for i, lib in ipairs({"comctl32", "rpcrt4", "winmm", "advapi32", "wsock32", "Dbghelp"}) do
					links { lib }
				end
			end
		end
    end

    -- use wx-config to figure out build parameters
    function wx_config_for_posix()
        local configCmd = "wx-config" -- this is the wx-config command line
        if wxRoot ~= "" then configCmd = path.join(wxRoot, "wx-config") end

        local function checkYesNo(value, option)
            if value == "" then return "" end
            if value == "yes" or value == "no" then return " --"..option.."="..value end
            error("wx"..option..' can only be "yes", "no" or empty' )
        end

        configCmd = configCmd .. checkYesNo(wxDebug,     "debug")
        configCmd = configCmd .. checkYesNo(wxStatic,    "static")
        configCmd = configCmd .. checkYesNo(wxUnicode,   "unicode")
        configCmd = configCmd .. checkYesNo(wxUniversal, "universal")
        if wxHost ~= "" then configCmd = configCmd .. " --host=" .. wxHost end
--      if wxVersion ~= "" then configCmd = configCmd .. " --version=" .. wxVersion end

		if _ACTION == "codelite" then
			-- set the parameters to the current configuration
			buildoptions {"$(shell " .. configCmd .." --cxxflags)"}

			if wxWithoutLibs == "no" then
				linkoptions  {"$(shell " .. configCmd .." --libs " .. wxLibs .. ")"}
			end

			if os.get() == "windows" then
				resoptions  {"$(shell " .. configCmd .." --rcflags)"}
			end
		else
			-- set the parameters to the current configuration
			buildoptions {"`" .. configCmd .." --cxxflags`"}

			if wxWithoutLibs == "no" then
				linkoptions  {"`" .. configCmd .." --libs " .. wxLibs .. "`"}
			end

			if os.get() == "windows" then
				resoptions  {"`" .. configCmd .." --rcflags || " .. configCmd .." --rescomp | cut -d' ' -f2-`"}
			end
		end
    end

-- BUG: here, using any configuration() function will reset the current filter
--      and apply configuration to all project configuration...
--      see http://industriousone.com/post/add-way-refine-configuration-filter
--      and http://sourceforge.net/tracker/?func=detail&aid=2936443&group_id=71616&atid=531881
--~     configuration "not windows"
--~         wx_config_for_posix()
--~     configuration "vs*"
--~         wx_config_for_windows("vc")
--~     configuration {"windows", "codeblocks or gmake or codelite"}
--~         wx_config_for_windows("gcc")
    if os.get() ~= "windows" then
        wx_config_for_posix()
    else
        local allowedCompiler = {"vc", "gcc"}

        if not table.contains( allowedCompiler, wxCompiler ) then
            print( "wrong wxWidgets Compiler specified('"..wxCompiler.."'), should be one of '".. table.concat(allowedCompiler, "', '").."'" )

            wxCompiler = "gcc"
        end
--~  BUG/WISH: I need a function like compiler.get() that return the project/configuration compiler
--~         local wxCompiler = "vc"
--~  BUG? --cc=compiler standard premake option is not registered in the _OPTIONS array
--~         if _OPTIONS and _OPTIONS["cc"] then
--~             wxCompiler = _OPTIONS.cc
--~             print("seen option '--cc=" .. _OPTIONS["cc"] .. "' overriding default cc='vc'")
--~         end
		if wxUseWXConfig == "no" then
			wx_config_for_windows(wxCompiler, wxCompilerVersion)
		else
			wx_config_for_posix()
		end
    end
end
