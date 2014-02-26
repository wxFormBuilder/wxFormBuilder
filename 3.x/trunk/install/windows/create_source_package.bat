@echo off
::**************************************************************************
:: File:           create_source_package.bat
:: Version:        1.06
:: Name:           RJP Computing 
:: Date:           03/15/2007
:: Description:    Creates a source directory so that the installer can
::                 include only the needed files.
:: Copyright (C) 2009 RJP Computing
::
:: This program is free software; you can redistribute it and/or
:: modify it under the terms of the GNU General Public License
:: as published by the Free Software Foundation; either version 2
:: of the License, or (at your option) any later version.
::
:: This program is distributed in the hope that it will be useful,
:: but WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
:: GNU General Public License for more details.
::
:: You should have received a copy of the GNU General Public License
:: along with this program; if not, write to the Free Software
:: Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
::**************************************************************************
set APP_VERSION=1.08
set APP_TITLE=Create Source Package

echo ----------------------------------------
echo       %APP_TITLE% v%APP_VERSION%
echo                   By
echo             RJP Computing
echo.
echo Creates a source directory so that the
echo installer can include only the needed
echo files.
echo.
echo             Copyright (c) 2010  
echo ----------------------------------------
echo.

:: Cleanup old source.
if exist source rmdir /S /Q source

:: Check to see if 'source' directory exists.
if exist source goto BEGIN_SVN_EXPORT

mkdir source
goto BEGIN_SVN_EXPORT

:BEGIN_SVN_EXPORT
:: Add Subversion install directory to the path.
set SVN_EXPORT="svn.exe" export --force
set SVN_REPOS=https://svn.code.sf.net/p/wxformbuilder/code/3.x/trunk

echo Using Subversion with command :
echo     %SVN_EXPORT%
echo.
echo Exporting wxFormBuilder v3.x source from :
echo     %SVN_REPOS%
echo.

echo [svn] Exporting workspace and premake scripts.
echo %SVN_EXPORT% --non-recursive %SVN_REPOS% source\
call %SVN_EXPORT% --non-recursive %SVN_REPOS% source\

echo [svn] Exporting 'output' directory to 'source\output'
call %SVN_EXPORT% %SVN_REPOS%/output source\output

echo [svn] Exporting 'plugins' directory to 'source\plugins'
call %SVN_EXPORT% %SVN_REPOS%/plugins source\plugins

echo [svn] Exporting 'sdk' directory to 'source\sdk'
call %SVN_EXPORT% %SVN_REPOS%/sdk source\sdk

echo [svn] Exporting 'src' directory to 'source\src'
call %SVN_EXPORT% %SVN_REPOS%/src source\src

echo [svn] Exporting 'build' directory to 'source\build'
call %SVN_EXPORT% %SVN_REPOS%/build source\build


goto END

:END
echo.
echo Finished creating source package...

:: Cleanup environment.
set APP_VERSION=
set APP_TITLE=
set SVN_EXPORT=
set SVN_REPOS=

