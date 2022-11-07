# wxFormBuilder

[![Windows Status](https://github.com/wxFormBuilder/wxFormBuilder/actions/workflows/windows.yml/badge.svg?branch=master)](https://github.com/wxFormBuilder/wxFormBuilder/actions/workflows/windows.yml)
[![Linux Status](https://github.com/wxFormBuilder/wxFormBuilder/actions/workflows/linux.yml/badge.svg?branch=master)](https://github.com/wxFormBuilder/wxFormBuilder/actions/workflows/linux.yml)
[![macOS Status](https://github.com/wxFormBuilder/wxFormBuilder/actions/workflows/macos.yml/badge.svg?branch=master)](https://github.com/wxFormBuilder/wxFormBuilder/actions/workflows/macos.yml)

wxFormBuilder is a GUI builder for the wxWidgets framework.

Code generation is supported for [C++](https://wxwidgets.org/), [Python](https://wxpython.org/),
[XRC](https://docs.wxwidgets.org/trunk/overview_xrc.html), [Lua](https://github.com/pkulchenko/wxlua) and [PHP](https://github.com/wxphp/wxphp).
Additionally, the import and export of XRC code is possible.
To support additional widgets, custom plugins can be used.

wxFormBuilder runs on Windows, various Linux distributions and macOS.

## Download Binaries

* [GitHub Releases](https://github.com/wxFormBuilder/wxFormBuilder/releases)
* [GitHub CI Builds](https://github.com/wxFormBuilder/wxFormBuilder/actions)

## Install from Source

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
_install/wxFormBuilder
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
sudo dnf install wxGTK-devel wxGTK-media boost-devel cmake make git
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

## Build custom plugins

For building custom wxFormBuilder plugins, refer to the [SDK documentation](./sdk/README.md).
