@echo off
::**************************************************************************
:: File:           create_install_files_pkg.bat
:: Version:        1.00
:: Name:           RJP Computing 
:: Date:           03/01/2007
:: Description:    Creates the install files package. This doesn't get the
::                 files from Subversion it just uses the export function.
::**************************************************************************
set APP_VERSION=1.00
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

:: Cleanup old source.
if exist files rmdir /S /Q files

:: Check to see if 'source' directory exists.
if exist files goto BEGIN_COPY

mkdir files

:BEGIN_COPY

echo Coping 'bin' directory to 'files'
xcopy ..\bin files /E /I /H /Y /EXCLUDE:excludes

goto END

:END
echo.
echo Finished creating install files package...

:: Cleanup environment.
set APP_VERSION=
set APP_TITLE=
