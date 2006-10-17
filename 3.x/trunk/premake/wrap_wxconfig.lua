------------------------------------------------------------------
-- File:		wrap_wxconfig.lua
-- Author:		RJP Computing
-- Date:		10/16/2006
-- Copyright:   (c) 2006 RJP Computing. All rights reserved.
-- Licence:     wxWidgets licence
-- Purpose:	This wrapes the functionality of wx-config for Windows
-- 			and Linux.
-- 
-- History:
-- 			v0.01	Initial concept.
------------------------------------------------------------------
local wxCfgPath = nil
local wxCfgCmd = nil

local wxCFG_DEBUG = 1		-- 0 = Debug info off, 1 = Debug info on.

-- Simple debug function. Use wxCFG_DEBUG to control the debug output.
-- wxCFG_DEBUG = 1 (Turns debugging information ON)
-- wxCFG_DEBUG = 0 (Turns debugging information OFF)
function wxCfgDebug( msg )
	if ( wxCFG_DEBUG == 1 ) then
		print( os.date() .. " | " .. msg )
	end
end

-- Setup a common path seperator for all platforms.
if ( OS == "windows" ) then
	wxCFG_PATH_SEP = "\\"
else
	wxCFG_PATH_SEP = "/"
end

wxCfgDebug( "Path separators = " .. wxCFG_PATH_SEP )

-- Initialize all the calls for wrapping wx-config.
-- @param	path Directory that wx-config is located.
-- @warning	Make sure to pass the end path separater. (ie. /tmp/foo/ not /tmp/foo)
-- @note	If @p path is empty it doesn't set the path at all.
-- 			It is assumed wx-config is in the PATH.
function wxCfgInit( path )
	if ( path == nil ) then
		-- Setup the wx-config path.
		wxCfgPath = ""
		-- Setup the command to use.
		wxCfgCmd = "wxconfig"
	else
		-- Setup the wx-config path.
		wxCfgPath = path
		wxCfgDebug( "wxconfig path = " .. wxCfgPath )				-- Debug info.
		
		-- Setup the command to use.
		wxCfgCmd = wxCfgPath .. "wxconfig"
	end
	wxCfgDebug( "Command to execute = " .. wxCfgCmd )			-- Debug info.
end

-- Gets the wxWidgets version number. Returns it in the form of (major).(minor)
-- @return wxWidgets major version number. (ie. 2.7)
function wxCfgGetWxVersion()
	local wxCfgWxWidgersVer = nil
	local tmpFile = "~wxcfgwxver"
	local inFile
	local testTmpFile
	
	-- Use file to get results. This is because windows doesn't support pipes easily.
	os.execute( wxCfgCmd .. " --release > " .. tmpFile )
	-- Open temp file.
	inFile = assert( io.open( tmpFile ) )
	-- read the line.
	wxCfgWxWidgersVer = inFile:read()
	-- Close the temp file.
	inFile:close()
	-- Delete the temp file.
	os.remove ( tmpFile )
	
	-- Debug info.
	wxCfgDebug( "wxWidgets version = " .. wxCfgWxWidgersVer )
	
	return wxCfgWxWidgersVer
end

-- Gets the wxWidgets version number. Returns it in the form of (major).(minor)
-- @return wxWidgets major version number. (ie. 2.7)
function wxCfgGetWxVersion()
	local wxCfgWxWidgersVer = nil
	local tmpFile = "~wxcfgwxver"
	
	-- Use file to get results. This is because windows doesn't support pipes easily.
	os.execute( wxCfgCmd .. " --release > " .. tmpFile )
	wxCfgWxWidgersVer = simPipeThruFile( tmpFile )
	
	-- Debug info.
	wxCfgDebug( "wxWidgets version = " .. wxCfgWxWidgersVer )
	
	return wxCfgWxWidgersVer
end

-- 
function wxCfgGetLibs()
	local wxCfgWxWidgetsLibs = nil
	local tmpFile = "~wxcfgwxlibs"
	
	-- Use file to get results. This is because windows doesn't support pipes easily.
	os.execute( wxCfgCmd .. " --libs > " .. tmpFile )
	wxCfgWxWidgetsLibs = simPipeThruFile( tmpFile )
	
	-- Debug info.
	wxCfgDebug( "wxWidgets libraries used to compile = " .. wxCfgWxWidgetsLibs )
	
	return wxCfgWxWidgetsLibs
end

-- 
function wxCfgGetCFlags()
	local wxCfgWxWidgetsCFlags = nil
	local tmpFile = "~wxcfgwxCFlags"
	
	-- Use file to get results. This is because windows doesn't support pipes easily.
	os.execute( wxCfgCmd .. " --cflags > " .. tmpFile )
	wxCfgWxWidgetsCFlags = simPipeThruFile( tmpFile )
	
	-- Debug info.
	wxCfgDebug( "wxWidgets compiler flags = " .. wxCfgWxWidgetsCFlags )
	
	return wxCfgWxWidgetsCFlags
end

-- Uses files to simulate pipes because Windows can't easily do pipes thru the console.
-- @param fileName Temperary file that will be opened and then deleted.
-- @return Files cotents.
-- @warning This function will delete the file passed to it.
function simPipeThruFile( fileName )
	local retVal = nil
	local inFile = nil
	
	-- Open temp file.
	inFile = assert( io.open( fileName ) )
	-- read the line.
	retVal = inFile:read()
	-- Close the temp file.
	inFile:close()
	-- Delete the temp file.
	os.remove ( fileName )
	
	return retVal
end

-- LINUX ONLY --------------------------------------------
-- Perform a shell command and return its output.
--   c: command
-- returns
--   o: output
function shell( c )
if ( OS ~= "windows" ) then
	local o, h
	h = assert(io.popen( c,"r" ))
	o = h:read("*all")
	h:close()

	return o
end
end

	