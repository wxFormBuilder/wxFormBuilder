rem This script is called during AppVeyor builds

if "%TOOLSET%"=="" set TOOLSET=cmake

rem Common preperation
git submodule update --init

rem run TOOLSET specifc commands
goto %TOOLSET%

:cmake_msys
if "%MSYSTEM%"=="" set MSYSTEM=MINGW32
C:\msys64\usr\bin\bash -lc "pacman -S --needed --noconfirm mingw-w64-i686-wxWidgets"
if ERRORLEVEL 1 goto error
path C:\msys64\%MSYSTEM%\bin;C:\msys64\usr\bin;%path%
set GENERATOR=MSYS Makefiles
goto cmake

:cmake_mingw
C:\msys64\usr\bin\bash -lc "pacman -S --needed --noconfirm mingw-w64-i686-wxWidgets"
if ERRORLEVEL 1 goto error
:: CMake requires a path without sh (added by git on AppVeyor)
path C:\Program Files (x86)\CMake\bin;C:\MinGW\bin
set GENERATOR=MinGW Makefiles
goto cmake

:cmake
echo --- Tools versions:
cmake --version

if "%TOOLSET%"=="cmake" (
    if "%WXVERSION%"=="" set WXVERSION=3.0.2
    echo --- Get wxWidgets binaries:
    call build\scripts\dl_wxwidgets.bat %WXVERSION% %WXCOMPILER%
    set WXWIN=%CD%\wxwidgets
)

mkdir build_cmake
pushd build_cmake

echo.
echo --- Generating project files
echo.
cmake -G "%GENERATOR%" ..
if ERRORLEVEL 1 goto error

echo.
echo --- Starting the build
echo.
cmake --build .
if ERRORLEVEL 1 goto error

popd
goto :eof

:mingw
cinst -y InnoSetup

C:\msys64\usr\bin\bash -lc "pacman -S --needed --noconfirm mingw-w64-i686-wxWidgets mingw-w64-i686-boost"
if ERRORLEVEL 1 goto error

call create_build_files4.bat --wx-root=/mingw32/bin --force-wx-config --disable-mediactrl
C:\msys64\usr\bin\bash -lc "ln -s /mingw32/include/binutils/bfd.h /mingw32/include/bfd.h"
C:\msys64\usr\bin\bash -lc "ln -s /mingw32/include/binutils/symcat.h /mingw32/include/symcat.h"
C:\msys64\usr\bin\bash -lc "ln -s /mingw32/lib/binutils/libbfd.a /mingw32/lib/libbfd.a"
C:\msys64\usr\bin\bash -lc "ln -s /mingw32/lib/binutils/libiberty.a /mingw32/lib/libiberty.a"
C:\msys64\usr\bin\bash -lc "cd '%cd%'/build/3.0/gmake && sed 's!\$(LDFLAGS) \$(RESOURCES) \$(ARCH) \$(LIBS)!\$(LIBS) \$(LDFLAGS) \$(RESOURCES) \$(ARCH)!g' *.make -i"
C:\msys64\usr\bin\bash -lc "cd '%cd%'/build/3.0/gmake && sed 's!-lbfd!-lbfd -lz!g' *.make -i"
C:\msys64\usr\bin\bash -lc "cd '%cd%'/build/3.0/gmake && make config=release"
if ERRORLEVEL 1 goto error

rem Build binary artifact
7z a -r wxFormBuilder_win32.zip %cd%\output\*.* C:\msys64\mingw32\bin\wx*.dll C:\msys64\mingw32\bin\libstdc++*.dll C:\msys64\mingw32\bin\libgcc*.dll C:\msys64\mingw32\bin\libintl*.dll C:\msys64\mingw32\bin\libexpat*.dll C:\msys64\mingw32\bin\libjpeg*.dll C:\msys64\mingw32\bin\libpng*.dll C:\msys64\mingw32\bin\libtiff*.dll C:\msys64\mingw32\bin\zlib*.dll C:\msys64\mingw32\bin\libwinpthread*.dll C:\msys64\mingw32\bin\libiconv*.dll C:\msys64\mingw32\bin\liblzma*.dll
appveyor PushArtifact wxFormBuilder_win32.zip

rem Build setup artifact
cd install/windows
create_source_package.bat
"C:\Program Files (x86)\Inno Setup 5\ISCC.exe" wxFormBuilder.iss
appveyor PushArtifact install\windows\wxFormBuilder*.exe

goto :eof

:error
echo.
echo --- Build failed !
echo.
