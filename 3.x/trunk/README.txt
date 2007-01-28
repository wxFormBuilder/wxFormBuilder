wxFormBuilder
-------------
Introduction:
	wxFormBuilder aims to be an application that as well as enabling visual
	development and generating the corresponding code, allow the inclusion of 
	non-graphical components, as well as providing facilities for extending the
	set of widgets easily via plugins, like other applications such as qt-designer.
	
	Use Premake, for your platform, to generate the build files of your choice.
	
	REMEMBER: The way you get new additions and updates to the build files you
	need to run	Premake. This is means that it is good practice to run Premake
	before starting	to build or develop wxFormBuilder.

Build Steps:
	- There are included batch/sh script that you can run to just simplify running
	  and creating the build files. Just run it to create the necessary build files.
	  You can skip the next step if you use those.
		* Files:
			Windows:	create_build_files.bat
			Linux:		create_build_files.sh
	  
	- To create the needed build files type:
	
		* Code::Blocks Projects and workspace:
			Windows:	premake\premake-win32.exe --target cb-gcc --with-wx-shared [--unicode]
			Linux:		premake/premake-linux --target cb-gcc --with-wx-shared [--unicode]
			
		* GNU makefiles:
			Windows:	premake\premake-win32.exe --target gnu --with-wx-shared [--unicode]
			Linux:		premake/premake-linux --target gnu --with-wx-shared [--unicode]

	- For Code::Blocks, use the generated wxFormBuilder.workspace to build wxFormBuilder.
	
	- For GNU makefiles type: (Assumes you have properly setup your system to build
	  with gcc or MinGW)
	  
		* Release:
			make CONFIG=Release
		
		* Debug:
			make

Includes:
	- wxFormBuilder
	
	- wxFormBuilder SDK
	
	- wxFormBuilder 3rd Party Controls

Notes:
	- Code::Blocks is a free cross-platform IDE and it can be found here:
	  http://codeblocks.org

	- Premake can be found here:
	  http://premake.sourceforge.net
	
	- Subversion is a great free cross-platform version control manager.
	  It can be found here:
	  http://subversion.tigris.org

Enjoy,
  The wxFormBuilder Team