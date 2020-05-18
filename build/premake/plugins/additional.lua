-----------------------------------------------------------------------------
--  Name:        additional.lua
--  Purpose:     Additional controls plugin project build script.
--  Author:      Andrea Zanellato
--  Modified by:
--  Created:     21/10/2011
--  Copyright:   (c) 2011 wxFormBuilder Team
--  Licence:     GNU General Public License Version 2
-----------------------------------------------------------------------------
project "additional-components-plugin"
    kind                "SharedLib"
    targetname          "additional"
    files               {"../../../plugins/additional/additional.cpp"}
    includedirs
    {
        "../../../subprojects/ticpp", "../../../sdk/plugin_interface"
    }
    defines             {"BUILD_DLL", "TIXML_USE_TICPP"}
    links               {"plugin-interface", "TiCPP"}

    local libs = "std,richtext,propgrid,stc,ribbon,aui"
    if wxUseMediaCtrl then
        defines         {"USE_MEDIACTRL"}
        libs            = libs .. ",media"
    end

    configuration "not vs*"
        buildoptions    "-std=c++17"

    configuration "vs*"
        buildoptions    "/std:c++17"

    configuration "not windows"
        targetdir       "../../../output/lib/wxformbuilder"

    configuration "windows"
        targetprefix    "lib"
        targetdir       "../../../output/plugins/additional"

    configuration "Debug"
        wx_config       {Libs=libs, Debug="yes"}
        targetsuffix    (DebugSuffix)

    configuration "Release"
        wx_config       {Libs=libs}

    configuration {"not vs*", "Release"}
        buildoptions    {"-fno-strict-aliasing"}
