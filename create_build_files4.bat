@ECHO OFF

REM Clear output
CLS

REM Set Defaults
SET unicode=
SET wxroot=%WXWIN%
SET wxver=3.0
SET compiler=gcc
SET compilerversion=
SET usewxconfig=

REM Handle parameters
:Loop
REM Show help and exit
IF [%1]==[-h]                   GOTO Help
IF [%1]==[--help]               GOTO Help
IF [%1]==[--compiler]           GOTO Compiler
IF [%1]==[--compiler-version]   GOTO CompilerVersion
IF [%1]==[--disable-mediactrl]  GOTO Mediactrl
IF [%1]==[--wx-root]            GOTO Root
IF [%1]==[--disable-unicode]    GOTO Unicode
IF [%1]==[--wx-version]         GOTO Version
IF [%1]==[--force-wx-config]    GOTO WXConfig
GOTO Premake

:Help
ECHO.
ECHO Available options:
ECHO.
ECHO --compiler             Specify compiler used.
ECHO                        Example: --compiler=vc if you use MSVC.
ECHO                        Current: %compiler%
ECHO.
ECHO --compilerversion      Specifies the version of the compiler used
ECHO                        Example: 48 for GCC 4.8 or 120 for VS2013
ECHO.
ECHO --disable-mediactrl    Disable wxMediaCtrl / wxMedia library.
ECHO                        Default: wxMediaCtrl enabled.
ECHO.
ECHO --disable-unicode      Whether to use an Unicode or an ANSI build.
ECHO                        Ignored in wxWidgets 2.9 and later.
ECHO                        Example: --disable-unicode produces an ANSI build.
ECHO                        Default: Unicode build on all versions.
ECHO.
ECHO --wx-root              Specify the wxWidgets build path,
ECHO                        useful for wxWidgets builds not installed
ECHO                        in your system (alternate/custom builds)
ECHO                        Example: --wx-root=D:\Devel\wxWidgets\3.0
ECHO                        Current: %WXWIN%
ECHO.
ECHO --wx-version           Specify the wxWidgets version.
ECHO                        Example: --wx-version=2.9
ECHO                        Default: %wxver%
ECHO
ECHO --force-wx-config      Force wx-config for the configuration
ECHO							even under MS Windows (usefull when CodeLite IDE
ECHO							is used for the building)
ECHO                        Default: Don't use wx-config under MS Windows
ECHO.
GOTO End

:Compiler
SET compiler=%2
SHIFT
SHIFT
GOTO Loop

:CompilerVersion
SET compilerversion=--compiler-version=%2
SHIFT
SHIFT
GOTO Loop

:Mediactrl
SET mediactrl=%1
SHIFT
GOTO Loop

:Root
SET wxroot=%2
SHIFT
SHIFT
GOTO Loop

:Unicode
SET unicode=%1
SHIFT
GOTO Loop

:WXConfig
SET usewxconfig=%1
SHIFT
GOTO Loop

:Version
SET wxver=%2
SHIFT
SHIFT
GOTO Loop

:Premake
build\premake\windows\premake4.exe --file=build/premake/solution.lua --wx-root=%wxroot% --wx-version=%wxver% --compiler=%compiler% %compilerversion% %unicode% %mediactrl% %usewxconfig% codelite
ECHO.

build\premake\windows\premake4.exe --file=build/premake/solution.lua --wx-root=%wxroot% --wx-version=%wxver% --compiler=%compiler% %compilerversion% %unicode% %mediactrl% %usewxconfig% gmake
ECHO.

ECHO Done generating all project files for wxFormBuilder.
:End

