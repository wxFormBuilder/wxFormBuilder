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
local wxConfigPath = nil
local wxConfigCmd = nil

local wxCFG_DEBUG = 1

-- Simple debug function. If you don't want to see messages switch
-- wxCFG_DEBUG = 1
-- to
-- wxCFG_DEBUG = 0
function wxCfgDebug( msg )
	if ( wxCFG_DEBUG == 1 ) then
		print( os.date() .. " : " .. msg )
	end
end

-- Setup a common path seperator for all platforms.
if ( OS == "windows" ) then
	wxCFG_PATH_SEP = "\\"
else
	wxCFG_PATH_SEP = "/"
end

wxCfgDebug( "Path separators = " .. wxCFG_PATH_SEP )

-- Initialize all the calls for wrapping wx-config
function wxCfgInit( path )
	if ( path == nil ) then
		wxConfigPath = "." .. wxCFG_PATH_SEP
	else
		wxConfigPath = path
	end
	-- Debug info.
	wxCfgDebug( "wxconfig path: " .. wxConfigPath )
	
	-- Setup the command to use.
	wxConfigCmd = wxConfigPath .. "wxconfig.exe"
	-- Debug info.
	wxCfgDebug( "Command to execute: " .. wxConfigCmd )
end

-- Gets the wxWidgets version number. Returns it in the form of (major).(minor)
-- @return wxWidgets major version number. (ie. 2.7)
function wxCfgGetWxVersion()
	local wxVer = os.execute( wxConfigCmd .. " --release" )
	
	-- Debug info.
	wxCfgDebug( "wxWidgets version : " .. wxVer )
	
	return wxVer
end
	