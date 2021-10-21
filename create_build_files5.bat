@ECHO OFF

REM Clear output
CLS

REM Set Defaults
SET shared=
SET monolithic=
SET wxroot=%WXWIN%
SET wxver=3.0
SET compiler=gcc
SET compilerversion=
SET architecture=
SET usewxconfig=no

REM Handle parameters
:Loop
REM Show help and exit
IF [%1]==[-h]                   GOTO Help
IF [%1]==[--help]               GOTO Help
IF [%1]==[--compiler]           GOTO Compiler
IF [%1]==[--compiler-version]   GOTO CompilerVersion
IF [%1]==[--architecture]       GOTO Architecture
IF [%1]==[--disable-mediactrl]  GOTO Mediactrl
IF [%1]==[--disable-shared]     GOTO Shared
IF [%1]==[--disable-monolithic] GOTO Monolithic
IF [%1]==[--wx-root]            GOTO Root
IF [%1]==[--wx-version]         GOTO Version
IF [%1]==[--force-wx-config]    GOTO WXConfig
GOTO Premake

:Help
ECHO.
ECHO Available options:
ECHO.
ECHO --compiler             Specify compiler used
ECHO                        Example: --compiler=vc if you use MSVC
ECHO                        Current: %compiler%
ECHO.
ECHO --compiler-version     Specifies the version of the compiler used
ECHO                        Example: 48 for GCC 4.8 or 120 for VS2013
ECHO.
ECHO --architecture         Specifies the architecture to build for
ECHO                        Example: x86_64 for 64 Bit
ECHO                        Default: Unset (use System Default)
ECHO.
ECHO --disable-mediactrl    Disable wxMediaCtrl / wxMedia library
ECHO                        Default: wxMediaCtrl enabled
ECHO.
ECHO --disable-shared       Disable usage of wxWidgets DLL and use Static Library
ECHO                        Default: DLL enabled
ECHO.
ECHO --disable-monolithic   Disable usage of Monolithic wxWidgets and use Components
ECHO                        Default: Use Monolithic
ECHO.
ECHO --wx-root              Specify the wxWidgets build path,
ECHO                        useful for wxWidgets builds not installed
ECHO                        in your system (alternate/custom builds)
ECHO                        Example: --wx-root=D:\Devel\wxWidgets\3.0
ECHO                        Current: %WXWIN%
ECHO.
ECHO --wx-version           Specify the wxWidgets version
ECHO                        Example: --wx-version=2.9
ECHO                        Default: %wxver%
ECHO.
ECHO --force-wx-config      Force wx-config for the configuration
ECHO                        even under MS Windows (useful when CodeLite IDE
ECHO                        is used for the building)
ECHO                        Example: --force-wx-config=yes
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

:Architecture
SET architecture=--architecture=%2
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

:Shared
SET shared=%1
SHIFT
GOTO Loop

:Monolithic
SET monolithic=%1
SHIFT
GOTO Loop

:WXConfig
SET usewxconfig=%2
SHIFT
SHIFT
GOTO Loop

:Version
SET wxver=%2
SHIFT
SHIFT
GOTO Loop

:Premake
scripts\premake\5.0\bin\release\premake5.exe --file=scripts/premake/solution.lua --force-wx-config=%usewxconfig% --wx-root=%wxroot% --wx-version=%wxver% --compiler=%compiler% %compilerversion% %architecture% %shared% %monolithic% %mediactrl% codelite
ECHO.

scripts\premake\5.0\bin\release\premake5.exe --file=scripts/premake/solution.lua --force-wx-config=%usewxconfig% --wx-root=%wxroot% --wx-version=%wxver% --compiler=%compiler% %compilerversion% %architecture% %shared% %monolithic% %mediactrl% gmake2
ECHO.

scripts\premake\5.0\bin\release\premake5.exe --file=scripts/premake/solution.lua --force-wx-config=%usewxconfig% --wx-root=%wxroot% --wx-version=%wxver% --compiler=%compiler% %compilerversion% %architecture% %shared% %monolithic% %mediactrl% vs2017
ECHO.

scripts\premake\5.0\bin\release\premake5.exe --file=scripts/premake/solution.lua --force-wx-config=%usewxconfig% --wx-root=%wxroot% --wx-version=%wxver% --compiler=%compiler% %compilerversion% %architecture% %shared% %monolithic% %mediactrl% vs2019
ECHO.

ECHO Done generating all project files for wxFormBuilder.
:End

