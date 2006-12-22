#!/bin/sh
#
premake/premake-linux --target cb-gcc --unicode --with-wx-shared
echo done...
echo 
#
premake/premake-linux --target gnu --unicode --with-wx-shared
echo done...
echo 
#
echo Done generating all project files for wxFormBuilder.
#