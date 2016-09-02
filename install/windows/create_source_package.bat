@echo off
::**************************************************************************
:: File:           create_source_package.bat
:: Version:        1.11
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
set APP_VERSION=1.11
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
if exist source goto BEGIN_GIT_EXPORT

mkdir source
goto BEGIN_GIT_EXPORT

:BEGIN_GIT_EXPORT
:: Add Subversion install directory to the path.
set GIT_EXPORT="git.exe" checkout-index -a -f --prefix=install/windows/source/
set GIT_REPOS=https://github.com/wxFormBuilder/wxFormBuilder.git

echo Using Git with command :
echo     %GIT_EXPORT%
echo.
echo Exporting wxFormBuilder v3.x source from :
echo     %GIT_REPOS%
echo.

echo [git] Exporting all source.
call pushd ..\..
call %GIT_EXPORT%
call popd

echo [git] Exporting 'sdk\tinyxml' directory to 'source\sdk\tinyxml'
call xcopy /hievry ..\..\sdk\tinyxml source\sdk\tinyxml

goto END

:END
echo.
echo Finished creating source package...

:: Cleanup environment.
set APP_VERSION=
set APP_TITLE=
set GIT_EXPORT=
set GIT_REPOS=

