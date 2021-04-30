-----------------------------------------------------------------------------
--  Name:        ticpp.lua
--  Purpose:     TinyXML project script.
--  Author:      RJP Computing <rjpcomputing@gmail.com>
--  Modified by: Andrea Zanellato zanellato.andrea@gmail.com
--  Created:     2008/21/01
--  Copyright:   (c) 2009 RJP Computing
--  Licence:     GNU General Public License Version 2
-----------------------------------------------------------------------------
project "TiCPP"
    kind                "StaticLib"
    targetname          "ticpp"
    targetdir           "../../sdk/lib"
    files               {"../../subprojects/ticpp/*.cpp", "../../subprojects/ticpp/*.h"}
    excludes            {"xmltest.cpp"}
    defines             {"TIXML_USE_TICPP"}

    configuration "not vs*"
        buildoptions    "-std=c++17"

    configuration "vs*"
        defines         {"_CRT_SECURE_NO_DEPRECATE", "_CRT_SECURE_NO_WARNINGS"}
        buildoptions    "/std:c++17"

    configuration "not windows"
        buildoptions    {"-fPIC"}

    configuration "Debug"
        targetsuffix    "d"
