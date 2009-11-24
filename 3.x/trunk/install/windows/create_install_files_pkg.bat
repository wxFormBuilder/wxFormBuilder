@echo off
::**************************************************************************
:: File:           create_install_files_pkg.bat
:: Version:        1.02
:: Name:           RJP Computing 
:: Date:           03/01/2007
:: Description:    Creates the install files package. This doesn't get the
::                 files from Subversion it just uses the export function.
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
::
:: Notes:          1.00 - Initial release.
::                 1.01 - Changed where to copy files from to match new
::                        layout.
::                 1.02 - Adding 'call' to the commands called.
::**************************************************************************
set APP_VERSION=1.02
set APP_TITLE=Create Install Files Package

echo ----------------------------------------
echo       %APP_TITLE% v%APP_VERSION%
echo                   By
echo             RJP Computing
echo.
echo Creates the install files package.
echo.
echo             Copyright (c) 2007  
echo ----------------------------------------
echo.

echo Current Directory: %CD%
 
:: Cleanup old source.
if exist files rmdir /S /Q files

:: Check to see if 'source' directory exists.
if exist files goto BEGIN_COPY

mkdir files

:BEGIN_COPY

echo Coping 'output' directory to 'files' Current Directory: %CD%
call xcopy ..\..\output files /E /I /H /Y /EXCLUDE:excludes

goto END

:END
echo.
echo Finished creating install files package...

:: Cleanup environment.
set APP_VERSION=
set APP_TITLE=
