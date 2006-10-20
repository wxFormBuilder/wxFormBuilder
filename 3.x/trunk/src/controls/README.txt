wxFormBuilder 3rd Party Controls
--------------------------------
Introduction:
	The wxFormBuilder 3rd Party Controls are a set of controls that are used
	internally by wxFormBuilder. They are a requirement for wxFormBuilder to build.
	You can just build the controls as its own package by using Premake, for your
	platform, to generate the build files of your choice. Now these projects are
	included in the main Premake script so it is not necessary to generate and
	build just the controls for wxFormBuilder.
	
	REMEMBER: The way you get new additions and updates to the build files you
	need to run	Premake. This is means that it is good practice to run Premake
	before starting	to build or develop wxFormBuilder.

Build Steps:
	- To create the needed build files navigate to the 'Premake' directory under the
	  wxFormBuilder directory structure and type:
	
		* Code::Blocks Projects and workspace:
			Windows:	premake-win32 --target cb-gcc --file src\controls\build\premake.lua
			Linux:		premake-linux --target cb-gcc --file src/controls/build/premake.lua
			
		* GNU makefiles:
			Windows:	premake-win32 --target gnu --file src\controls\build\premake.lua
			Linux:		premake-linux --target gnu --file src/controls/build/premake.lua
	
	- Navigate to '../src/controls/build' directory.

	- For Code::Blocks, use the generated .workspace to build the 3rd Party controls.
	
	- For GNU makefiles type: (Assumes you have properly setup your system to build
	  with gcc or MinGW)
	  
		* Release:
			make CONFIG=Release
		
		* Debug:
			make

Includes:
	1. wxFlatNotebook
		Used to give a good look and feel to the tabs in the wxFormBuilder user
		interface.
	
	2. wxPropGrid
		Used to show all the properties of all the controls that can be placed
		in wxFormBuilder.
	
	3. wxScintilla
		Used to display the C++ and xrc code. It has syntax highlighting.

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