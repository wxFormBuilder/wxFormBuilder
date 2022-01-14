# wxFormBuilder

[![Windows Status](https://github.com/wxFormBuilder/wxFormBuilder/actions/workflows/windows.yml/badge.svg?branch=master)](https://github.com/wxFormBuilder/wxFormBuilder/actions/workflows/windows.yml)
[![Linux Status](https://github.com/wxFormBuilder/wxFormBuilder/actions/workflows/linux.yml/badge.svg?branch=master)](https://github.com/wxFormBuilder/wxFormBuilder/actions/workflows/linux.yml)
[![macOS Status](https://github.com/wxFormBuilder/wxFormBuilder/actions/workflows/macos.yml/badge.svg?branch=master)](https://github.com/wxFormBuilder/wxFormBuilder/actions/workflows/macos.yml)

wxFormBuilder is a GUI builder for the wxWidgets framework.

Code generation is supported for C++, Python, XRC, wxLua and PHP.
Additionally, the import and export of XRC code is possible.
To support additional widgets custom plugins can be used.

wxFormBuilder runs on Windows, various Linux distributions and macOS.

## Download Binaries

* [GitHub Releases](https://github.com/wxFormBuilder/wxFormBuilder/releases)
* [GitHub CI Builds](https://github.com/wxFormBuilder/wxFormBuilder/actions)

## Install from Source with CMake

Building from source requires the fairly recent CMake version 3.21. Most Linux distributions don't contain this version
in their package repositories currently, the [CMake](https://cmake.org/download/) website offers binary downloads for
multiple platforms. wxFormBuilder uses the [wxWidgets](https://wxwidgets.org/) framework itself, it is highly recommended using
the current development version 3.1.x. The latest stable version 3.0.x is known to cause multiple issues and should be avoided.

### Windows

Building on Windows has been tested with [MSVC](https://visualstudio.com), [Mingw-w64](https://mingw-w64.org) and
[MSYS2](https://msys2.org) in 32 bit and 64 bit mode. Using MSYS2 is a bit more convenient because it offers the required
libraries precompiled and CMake can find them automatically. Using the other compilers it might be necessary to specify the library
locations manually. The following instructions use MSYS2.

Install MSYS2 and open a MINGW32 or MINGW64 shell.

Installing the Prerequisites:

```sh
pacman -Syu
pacman -S ${MINGW_PACKAGE_PREFIX}-toolchain ${MINGW_PACKAGE_PREFIX}-cmake ${MINGW_PACKAGE_PREFIX}-make base-devel git
pacman -S ${MINGW_PACKAGE_PREFIX}-wxWidgets3.1 ${MINGW_PACKAGE_PREFIX}-boost
```

Building:

```sh
git clone --recursive https://github.com/wxFormBuilder/wxFormBuilder
cd wxFormBuilder
cmake -S . -B _build -G "MSYS Makefiles" --install-prefix "$PWD/_install" -DCMAKE_BUILD_TYPE=Release
cmake --build _build --config Release -j `nproc`
cmake --install _build --config Release
```

Running:

```sh
_install/bin/wxFormBuilder
```
build msvc

Installing the Prerequisites:
download Development Files
```sh
https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.5/wxMSW-3.1.5_vc14x_Dev.7z
```
download header Files
```sh
https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.5/wxWidgets-3.1.5-headers.7z
```
Once you have the files you need, unzip all of them into the same directory, for example D:\Temp\wxMSW-3.1.5_vc14x_Dev. You should have only include and lib subdirectories under it, nothing else. 

Building:
```sh
create_build_files5.bat --compiler=vc --compiler-version=14x --disable-monolithic --wx-root=D:\Temp\wxMSW-3.1.5_vc14x_Dev --wx-version=3.1
```

---

### Linux

Building on Linux has been tested on Ubuntu and Fedora with GCC in 64 bit mode but should work on many more distributions.

#### Ubuntu

Installing the Prerequisites:

```sh
sudo apt install libwxgtk3.0-gtk3-dev libwxgtk-media3.0-gtk3-dev libboost-dev cmake make git
```

Building:

```sh
git clone --recursive https://github.com/wxFormBuilder/wxFormBuilder
cd wxFormBuilder
cmake -S . -B _build -G "Unix Makefiles" --install-prefix "$PWD/_install" -DCMAKE_BUILD_TYPE=Release -DWXFB_WXWIDGETS_ENABLE_30=ON
cmake --build _build --config Release -j `nproc`
cmake --install _build --config Release
```

Running:

```sh
_install/bin/wxformbuilder
```

#### Fedora

Installing the Prerequisites:

```sh
sudo dnf install wxGTK wxGTK-media boost-devel cmake make git
```

Building:

```sh
git clone --recursive https://github.com/wxFormBuilder/wxFormBuilder
cd wxFormBuilder
cmake -S . -B _build -G "Unix Makefiles" --install-prefix "$PWD/_install" -DCMAKE_BUILD_TYPE=Release
cmake --build _build --config Release -j `nproc`
cmake --install _build --config Release
```

Running:

```sh
_install/bin/wxformbuilder
```

---

### macOS

Building on macOS has been tested with Clang in 64 bit mode using Xcode and make. The required libraries can be installed
via [Homebrew](https://brew.sh/).

Installing the Prerequisites:

```sh
brew update
brew install wxwidgets boost cmake make git
```

Building:

```sh
git clone --recursive https://github.com/wxFormBuilder/wxFormBuilder
cd wxFormBuilder
cmake -S . -B _build -G "Unix Makefiles" --install-prefix "$PWD/_install" -DCMAKE_BUILD_TYPE=Release
cmake --build _build --config Release -j `sysctl -n hw.ncpu`
cmake --install _build --config Release
```

Running:

```sh
open _install/wxFormBuilder.app
```

## Install from Source with Premake/Meson (deprecated)

Previously Premake and Meson have been used for building but support for these build systems has been dropped
in favor for CMake. Building with these systems is more difficult and the result doesn't work on every platform
without issues. Both systems will be removed in the future, the documentation is kept for reference until that time.

### Windows (MSYS2)

Install [MSYS2](https://www.msys2.org/) and run the following inside a MinGW 32 bit shell:

```sh
pacman -Syu
pacman -S --needed mingw-w64-i686-gcc mingw-w64-i686-wxWidgets3.1 make git
git clone --recursive --depth=1 https://github.com/wxFormBuilder/wxFormBuilder
cd wxFormBuilder
cmd.exe /C "create_build_files4.bat --wx-root=/mingw32/bin --force-wx-config=versioned --wx-version=3.1"
ln -s /mingw32/include/binutils/ansidecl.h /mingw32/include/ansidecl.h
ln -s /mingw32/include/binutils/bfd.h /mingw32/include/bfd.h
ln -s /mingw32/include/binutils/diagnostics.h /mingw32/include/diagnostics.h
ln -s /mingw32/include/binutils/symcat.h /mingw32/include/symcat.h
ln -s /mingw32/lib/binutils/libbfd.a /mingw32/lib/libbfd.a
ln -s /mingw32/lib/binutils/libiberty.a /mingw32/lib/libiberty.a
cd build/3.1/gmake
sed 's!\$(LDFLAGS) \$(RESOURCES) \$(ARCH) \$(LIBS)!\$(LIBS) \$(LDFLAGS) \$(RESOURCES) \$(ARCH)!g' *.make -i
make config=release
```

Run:

```sh
cd ../../../output/
./wxFormBuilder.exe
```

### Linux

**Notice:** Distributions that use architecture based subdirectories for libraries currently fail to load the plugins, for a workaround see [#524](https://github.com/wxFormBuilder/wxFormBuilder/issues/524)

Pre-requisites for Ubuntu:

```sh
sudo apt install libwxgtk3.0-gtk3-dev libwxgtk-media3.0-gtk3-dev meson
```

Pre-requisites for Arch Linux:

```sh
sudo pacman -Syu --needed meson wxgtk2
```

Build and run:

```sh
git clone --recursive --depth=1 https://github.com/wxFormBuilder/wxFormBuilder
cd wxFormBuilder
meson _build --prefix $PWD/_install --buildtype=release
ninja -C _build install
./_install/bin/wxformbuilder
```

### macOS

**Notice:** On recent macOS versions >= 11 wxFormBuilder fails to build and/or run, there is no solution available currently

Pre-requisites for macOS can be installed via [Homebrew](https://brew.sh/):

```sh
brew install dylibbundler make wxwidgets
```

Note: Building with Xcode currently does not work without issues (see [#247](https://github.com/wxFormBuilder/wxFormBuilder/issues/247)). Therefore, it is recommended to build with GNU make like as described below:

```sh
git clone --recursive --depth=1 https://github.com/wxFormBuilder/wxFormBuilder
cd wxFormBuilder
./create_build_files4.sh
cd build/3.1/gmake
make config=release
```

Run:

```sh
cd ../../../output/
open wxFormBuilder.app
```
