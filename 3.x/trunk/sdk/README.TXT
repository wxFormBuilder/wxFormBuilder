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
	
Build File Creation:
	To create the needed build files navigate to the 'sdk' directory and type
	one of the following commands:

		- Both Code::Blocks and GNU Makefiles:
			There are included batch/sh script that you can run to just simplify
			running	and creating the build files. Just run it to create the
			necessary build files. You can jump to "Build Steps:" if you use these.

			Windows:	create_build_files.bat
			Linux:		create_build_files.sh
	
		- Code::Blocks Projects and workspace:
			Windows:	premake\premake-win32 --target cb-gcc --with-wx-shared [--unicode]
			Linux:		premake/premake-linux --target cb-gcc --with-wx-shared [--unicode]
			
		- GNU makefile:
			Windows:	premake\premake-win32 --target gnu --with-wx-shared [--unicode]
			Linux:		premake/premake-linux --target gnu --with-wx-shared [--unicode]

Build Steps:
	- For Code::Blocks, use the generated .workspace to build the SDK.
	
	- For GNU makefiles type: (Assumes you have properly setup your system to build
	  with gcc or MinGW)
	  
		* Release:
			make CONFIG=Release
		
		* Debug:
			make

Includes:

	1. plugin_interface
		wxFormBuilder has a plug-in interface and every plug-in shares this as a
		dependancy. It builds to a static library and should be included in any
		and all plug-ins.
    
	2. Tinyxml++ 2.4.2 (TiCPP)
		wxFormBuilder uses TinyXML++ for writing and reading xml.
		
		You can find TinyXML++ on http://code.google.com/p/ticpp/
	
	3. Premake
		The build file generator. It is precompiled for Windows, Linux, and Mac.

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

Enjoy,
  The wxFormBuilder Team