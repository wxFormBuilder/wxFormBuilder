wxFormBuilder Plugin SDK
------------------------
Introduction:
	The wxFormBuilder SDK makes it simple to create plug-ins for wxFormBuilder.
	It is a set of needed libraries that every plug-in must use to be considered
	a wxFormBuilder plug-in. The SDK uses a simple but effective build tool called
	Premake. You use premake to build makefiles, Visual Studio 7.1/2005 projects,
	and Code::Blocks projects. Now makefile and Code::Blocks are the supported
	types that the wxFormBuilder team tests and uses, so your mileage may vary
	with the other build types. 
	
	To create the needed build types navigate to the 'sdk' directory under the
	wxFormBuilder directory structure and type:
	
		1. Code::Blocks Projects and workspace:
			premake-win32 --target cb-gcc
			
		2. Linux makefile:
			premake --target gnu

Includes:

	1. plugin_interface
		wxFormBuilder has a plug-in interface and every plug-in shares this as a
		dependancy. It builds to a static library and should be included in any
		and all plug-ins.
		
		You can build it use the included Code::Blocks project (.cbp). 
		There will be makefile support soon.
    
	2. Tinyxml++ 2.4.2 (TiCPP)
		wxFormBuilder uses TinyXML++ for writing and reading xml. For building TinyXML++
		use the included Code::Blocks project (.cbp). There will be makefile support soon.
		
		You can find TinyXML++ on http://code.google.com/p/ticpp/

Notes:
	- Code::Blocks is a free cross-platform IDE and it can be found here:
	  http://codeblocks.org

	- Premake can be found here:
	  http://premake.sourceforge.net
	
	- Subversion is a great free cross-platform version control manager.
	  It can be found here:
	  http://subversion.tigris.org
	  
	- TinyXML++ (TiCPP) is a great small xml parser. It can be found here:
	  http://code.google.com/p/ticpp/

	- I suggest you use the Code::Blocks workspace because it will place everything where
	  it belongs. No need to worry about the files being located in the proper location.

	- Makefiles will be coming in the future.

Enjoy,
  The wxFormBuilder Team