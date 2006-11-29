@echo off
::**************************************************************************
:: File:           build_controls.bat
:: Version:        1.00
:: Name:           RJP Computing 
:: Date:           09/19/2006
:: Description:    Use this to build all the projects for wxFormBuilder
::                 3rd Party controls.
::                 Make sure to add the calls to any additions made to the
::                 controls library.
::**************************************************************************

echo Generate all makefiles needed.
echo.
::call bakefile_gen

:: -- wxPropGrid --
echo Building wxPropGrid with MinGW Gcc
echo.
cd propgrid
call wxBuild_default MINGW ALL
cd ..

:: -- wxFlatNotebook --
echo Building wxFlatNotebook with MinGW Gcc
echo.
cd wxFlatNotebook
call wxBuild_wxFlatNotebook MINGW ALL
cd ..

:: -- wxScintilla --
echo Building wxScintilla with MinGW Gcc
echo.
cd wxScintilla
call wxBuild_default MINGW ALL
cd ..

::echo Clean up link libraries for MinGW Gcc.
::cd ..\lib\gcc_dll
::del /Q /F /S *.a
::cd ..\build

echo Done building wxFormBuilder 3rd Party controls.
