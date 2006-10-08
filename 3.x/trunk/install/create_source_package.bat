@echo off
::**************************************************************************
:: File:           create_source_package.bat
:: Version:        1.01
:: Name:           RJP Computing 
:: Date:           07/24/2006
:: Description:    creates a source directory so that the installer can
::                 include only the needed files.
::**************************************************************************

echo Cleaning...
echo.
if exist source rmdir /S /Q source

echo Coping 'bin' directory to 'source\bin'
xcopy ..\bin source\bin /E /I /H /Y /EXCLUDE:excludes

echo Coping 'plugins' directory to 'source\plugins'
xcopy ..\plugins source\plugins /E /I /H /Y /EXCLUDE:excludes

echo Coping 'sdk' directory to 'source\libs'
xcopy ..\sdk source\sdk /E /I /H /Y /EXCLUDE:excludes
copy /Y ..\sdk\premake-win32.exe source\sdk

echo Coping 'src' directory to 'source\src'
xcopy ..\src source\src /E /I /H /Y /EXCLUDE:excludes

echo.
echo Finished source package...
