@echo off
::**************************************************************************
:: File:           export_bin_dir.bat
:: Version:        1.00
:: Name:           RJP Computing 
:: Date:           02/28/2007
:: Description:    Exports the 'bin' directory that was just built. This
::                 doesn't get the files from Subversion it just uses the
::                 export function.
::**************************************************************************
set APP_VERSION=1.00
set APP_TITLE=Export Bin Directory

echo ----------------------------------------
echo       %APP_TITLE% v%APP_VERSION%
echo                   By
echo             RJP Computing
echo.
echo Exports the 'bin' directory that was
echo just built.
echo.
echo             Copyright (c) 2007  
echo ----------------------------------------
echo.

:: Cleanup old source.
if exist files rmdir /S /Q files

:: Check to see if 'source' directory exists.
if exist files goto BEGIN_SVN_EXPORT

mkdir files
goto BEGIN_SVN_EXPORT

:BEGIN_SVN_EXPORT
:: Add Subversion install directory to the path.
set SVN_ROOT=C:\Program Files\Subversion
set SVN_EXPORT="%SVN_ROOT%\bin\svn.exe" export --force
set SVN_REPOS=https://wxformbuilder.svn.sourceforge.net/svnroot/wxformbuilder/3.x/trunk

echo Using Subversion with command :
echo     %SVN_EXPORT%
echo.
echo Exporting wxFormBuilder v3.x source from :
echo     %SVN_REPOS%
echo.

echo [svn] Exporting workspace and premake scripts.
%SVN_EXPORT% --non-recursive %SVN_REPOS% source\

echo [svn] Exporting 'bin' directory to 'source\bin'
%SVN_EXPORT% %SVN_REPOS%/bin source\bin

echo [svn] Exporting 'plugins' directory to 'source\plugins'
%SVN_EXPORT% %SVN_REPOS%/plugins source\plugins

echo [svn] Exporting 'premake' directory to 'source\premake'
%SVN_EXPORT% %SVN_REPOS%/premake source\premake

echo [svn] Exporting 'sdk' directory to 'source\sdk'
%SVN_EXPORT% %SVN_REPOS%/sdk source\sdk

echo [svn] Exporting 'src' directory to 'source\src'
%SVN_EXPORT% %SVN_REPOS%/src source\src


goto END

:END
echo.
echo Finished creating source package...

:: Cleanup environment.
set APP_VERSION=
set APP_TITLE=
set SVN_ROOT=
set SVN_EXPORT=
set SVN_REPOS=

