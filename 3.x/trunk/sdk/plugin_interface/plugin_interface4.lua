--*****************************************************************************
--*	Author:		Chris Steenwyk <csteenwyk@gmail.com>
--*	Date:		11/19/2013
--*	Version:	1.00
--*
--*	NOTES:
--*		- use the '/' slash for all paths.
--*****************************************************************************

-- GENERAL SETUP -------------------------------------------------------------
--
project( "plugin-interface" )
targetname( "fbPluginInterface" )
kind	"StaticLib"

function CommonSetup()
	defines			{
						"TIXML_USE_TICPP",
					}
	includedirs 	{
						"../tinyxml",
					}
	files			{ 
						"**.cpp",
						"**.h",
						"**.rc",
						"**.fpb",
						"*.lua",
					}
	Configure()
end

CommonSetup()
wx.Configure( true )
