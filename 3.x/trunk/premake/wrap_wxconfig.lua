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
--			v0.10	Added GetWxVersion().
--			v0.20	Added GetLibs(), GetCFlags().
--			v0.30	Added a generic Execute() function so that 
--			v0.40	Added object oriented class structure.
------------------------------------------------------------------

-- ///////////////////////////////////////////////////////////////
-- // CLASS DECLARATION //////////////////////////////////////////
-- ///////////////////////////////////////////////////////////////
-- Class constructor
wxCfg = 
{
	wxCfgPath,
	wxCfgCmd,
	wxCFG_DEBUG = 1		-- 0 = Debug info off, 1 = Debug info on.
}

-- ////////////////////////////////////////////////////////////////
-- // WXCFG METHODS ///////////////////////////////////////////////
-- ////////////////////////////////////////////////////////////////

-- Use this to be able to call functions as an object instead of just
-- a set of scoped static functions.
function wxCfg:new()
	return setmetatable( { wxCfgPath = nil, wxCfgCmd = nil, wxCFG_DEBUG = 1 }, mt )
end

-- Initialize all the calls for wrapping wx-config.
-- @param	path Directory that wx-config is located.
-- @warning	Make sure to pass the end path separater. (ie. /tmp/foo/ not /tmp/foo)
-- @note	If @p path is empty it doesn't set the path at all.
-- 			It is assumed wx-config is in the PATH.
function wxCfg:Init( path )
	if ( path == nil ) then
		-- Setup the wx-config path.
		wxCfgPath = ""
		-- Setup the command to use.
		wxCfgCmd = "wx-config"
	else
		-- Setup the wx-config path.
		wxCfgPath = path
		self:Debug( "wx-config path = " .. wxCfgPath )				-- Debug info.
		
		-- Setup the command to use.
		wxCfgCmd = wxCfgPath .. "wx-config"
	end
	self:Debug( "Command to execute = " .. wxCfgCmd )			-- Debug info.
end

-- Gets the wxWidgets version number. Returns it in the form of (major).(minor)
-- @return wxWidgets major version number. (ie. 2.7)
function wxCfg:GetWxVersion()
	local wxCfgWxWidgersVer = nil
	
	-- Use file to get results. This is because windows doesn't support pipes easily.
	wxCfgWxWidgersVer = self:Execute( "--release" )
	
	-- Debug info.
	self:Debug( "wxWidgets version = " .. wxCfgWxWidgersVer )
	
	return wxCfgWxWidgersVer
end

-- Gets all the linker flags.
-- @returns The linker flags.
-- @todo Needs to take in a build type.
function wxCfg:GetLibs()
	local wxCfgWxWidgetsLibs = nil
	
	wxCfgWxWidgetsLibs = self:Execute( "--libs" )
	
	-- Debug info.
	self:Debug( "wxWidgets libraries used to compile = " .. wxCfgWxWidgetsLibs )
	
	return wxCfgWxWidgetsLibs
end

-- Gets all the pre-processor and compiler flags.
-- @returns The pre-processor and compiler flags.
-- @todo Needs to take in a build type.
function wxCfg:GetCFlags()
	local wxCfgWxWidgetsCFlags = nil
	
	wxCfgWxWidgetsCFlags = self:Execute( "--cflags" )
	
	-- Debug info.
	self:Debug( "wxWidgets compiler flags = " .. wxCfgWxWidgetsCFlags )
	
	return wxCfgWxWidgetsCFlags
end

-- Calls wx-config with the specified command.
-- @param cmd Command to send to wx-config. (ie. "--unicode=yes --libs")
-- @return The output from wx-config.
function wxCfg:Execute( cmd )
	local tmpFile = "~wxcfgtmp"
	
	-- Use file to get results. This is because windows doesn't support pipes easily.
	os.execute( wxCfgCmd .. " " .. cmd .. " > " .. tmpFile )
	return self:simPipeThruFile( tmpFile )
end

-- Uses files to simulate pipes because Windows can't easily do pipes thru the console.
-- @param fileName Temperary file that will be opened and then deleted.
-- @return Files cotents.
-- @warning This function will delete the file passed to it.
function wxCfg:simPipeThruFile( fileName )
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

-- Simple debug function. Use wxCFG_DEBUG to control the debug output.
-- wxCFG_DEBUG = 1 (Turns debugging information ON)
-- wxCFG_DEBUG = 0 (Turns debugging information OFF)
function wxCfg:Debug( msg )
	if ( self.wxCFG_DEBUG == 1 ) then
		print( os.date() .. " | " .. msg )
	end
end

-- ///////////////////////////////////////////////////////////////
-- // HELPER METHODS /////////////////////////////////////////////
-- ///////////////////////////////////////////////////////////////
-- Setup a common path seperator for all platforms.
if ( OS == "windows" ) then
	wxCFG_PATH_SEP = "\\"
else
	wxCFG_PATH_SEP = "/"
end

wxCfg:Debug( "Path separators = " .. wxCFG_PATH_SEP )

-- ///////////////////////////////////////////////////////////////
-- // LUA METATABLE SETUP ////////////////////////////////////////
-- ///////////////////////////////////////////////////////////////
-- // Should not need to edit this.
-- // Used to make calling like a class possible.
mt = {}
-- Setup the ability to call functions in an object oreinted way not a static function way.
mt.__index = wxCfg -- redirect queries to the wxCfg table.
-- // LUA METATABLE SETUP END ////////////////////////////////////
