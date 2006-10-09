@echo off
::**************************************************************************
:: File:           create_source_package.bat
:: Version:        1.02
:: Name:           RJP Computing 
:: Date:           07/24/2006
:: Description:    creates a source directory so that the installer can
::                 include only the needed files.
::**************************************************************************

echo Cleaning...
echo.
if exist source rmdir /S /Q source

:: Check to see if 'source' directory exists.
if exist source goto BEGIN_COPY

mkdir source

:BEGIN_COPY

echo Coping workspace and premake scripts.
copy /Y ..\* .\source\

echo Coping 'bin' directory to 'source\bin'
xcopy ..\bin source\bin /E /I /H /Y /EXCLUDE:excludes

echo Coping 'plugins' directory to 'source\plugins'
xcopy ..\plugins source\plugins /E /I /H /Y /EXCLUDE:excludes

echo Coping 'sdk' directory to 'source\libs'
xcopy ..\sdk source\sdk /E /I /H /Y /EXCLUDE:excludes
copy /Y ..\sdk\premake-win32.exe source\sdk

echo Coping 'src' directory to 'source\src'
xcopy ..\src source\src /E /I /H /Y /EXCLUDE:excludes
goto END

:END
echo.
echo Finished creating source package...
