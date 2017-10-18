--****************************************************************************08
--*	Author:		RJP Computing <rjpcomputing@gmail.com>
--*	Date:		2/9/2008
--*	Version:	1.00-beta
--* Copyright (C) 2008 RJP Computing
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
--* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
--*
--*	NOTES:
--*		- use the '/' slash for all paths.
--*****************************************************************************
project.name = "wxFormBuilder SDK"

-- Add sdk projects here. (This needs to match the directory name not the package name.)
dopackage( "plugin_interface" )
dopackage( "tinyxml" )

-- Add options here.
addoption( "dynamic-runtime", "Use the dynamicly loadable version of the runtime." )
addoption( "unicode", "Use the Unicode character set" )

