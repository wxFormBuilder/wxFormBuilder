--*****************************************************************************
--*	Author:		RJP Computing <rjpcomputing@gmail.com>
--*	Date:		12/15/2006
--*	Version:	1.00-beta
--* Copyright (C) 2006 RJP Computing
--*
--* This program is free software; you can redistribute it and/or
--* modify it under the terms of the GNU General Public License
--* as published by the Free Software Foundation; either version 2
--* of the License, or (at your option) any later version.
--*
--* This program is distributed in the hope that it will be useful,
--* but WITHOUT ANY WARRANTY; without even the implied warranty of
--* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--* GNU General Public License for more details.
--*
--* You should have received a copy of the GNU General Public License
--* along with this program; if not, write to the Free Software
--* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
--*
--*	NOTES:
--*		- use the '/' slash for all paths.
--*****************************************************************************
project.name = "wxFormBuilder 3rd-Party Controls"

project.bindir = "../../../output"
project.libdir = "../../../output"

--dofile( "../../../premake/wrap_wxconfig.lua" )

-- Path to wx-config.
--wxconfigPath = ".." .. wxCFG_PATH_SEP .. ".." .. wxCFG_PATH_SEP .. ".." .. wxCFG_PATH_SEP .. "premake" .. wxCFG_PATH_SEP
-- Create a wxCfg object.
--cfg = wxCfg:new()
--cfg:Init( wxconfigPath )

--Test out functionality
--print( "wxWidgets version = " .. cfg:GetWxVersion() )
--print( "wxWidgets Libs = " .. cfg:GetLibs() )
--print( "wxWidgets cflags = " .. cfg:GetCFlags() )

-- Add sdk projects here.
dopackage( "propgrid" )
dopackage( "wxFlatNotebook" )
dopackage( "wxScintilla" )
