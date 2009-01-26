@echo off

sdk\premake\premake-win32.exe --target cl-gcc --unicode --with-wx-shared
echo done...
echo.

sdk\premake\premake-win32.exe --target gnu --unicode --with-wx-shared
echo done...
echo.

echo Done generating all project files for wxFormBuilder.