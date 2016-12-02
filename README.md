# wxFormBuilder [![Build status](https://ci.appveyor.com/api/projects/status/yxpn19g0st7l9r8x?svg=true)](https://ci.appveyor.com/project/jhasse/wxformbuilder-461d5)

## Install From Repositories

0. Add PPA for wxWidgets v3.0 (Ubuntu 12.04-13.10)

	```sh
	sudo add-apt-repository -y ppa:wxformbuilder/wxwidgets
	sudo apt-get update
	```
1. Pre-requisites (Ubuntu 12.04 LTS)

	```sh
	sudo apt-get install libwxgtk3.0-0 libwxgtk-media3.0-0
	```
2. Add PPA for wxFormBuilder (Ubuntu 12.04-13.10)

	```sh
	sudo add-apt-repository -y ppa:wxformbuilder/release
	sudo apt-get update
	```
3. Install wxFormBuilder

	```sh
	sudo apt-get install wxformbuilder
	```

## Install From Source

### Windows (MSYS2)

Install [MSYS2](http://msys2.github.io/) and run the following inside a MinGW 32 bit shell:

```sh
pacman -S --needed mingw-w64-i686-wxWidgets git make mingw-w64-i686-binutils mingw-w64-i686-gcc
git clone --recursive --depth=1 https://github.com/wxFormBuilder/wxFormBuilder
cd wxFormBuilder
cmd.exe /C "create_build_files4.bat --wx-root=/mingw32/bin --force-wx-config --disable-mediactrl"
ln -s /mingw32/include/binutils/bfd.h /mingw32/include/bfd.h
ln -s /mingw32/include/binutils/symcat.h /mingw32/include/symcat.h
ln -s /mingw32/lib/binutils/libbfd.a /mingw32/lib/libbfd.a
ln -s /mingw32/lib/binutils/libiberty.a /mingw32/lib/libiberty.a
cd build/3.0/gmake
sed 's!\$(LDFLAGS) \$(RESOURCES) \$(ARCH) \$(LIBS)!\$(LIBS) \$(LDFLAGS) \$(RESOURCES) \$(ARCH)!g' *.make -i
sed 's!-lbfd!-lbfd -lz!g' *.make -i
make config=release
```

### Ubuntu Linux

0. Add PPA for wxWidgets v3.0 (Ubuntu 12.04-13.10)

	```sh
	sudo add-apt-repository -y ppa:wxformbuilder/wxwidgets
	```
1. Pre-requisites (Ubuntu 12.04 LTS)

	```sh
	sudo apt-get install libwxgtk3.0-0 libwxgtk3.0-dev libwxgtk-media3.0-dev
	```
2. Download source code

	```sh
	cd ~/src/
	git clone https://github.com/wxFormBuilder/wxFormBuilder.git
	```
3. Prepare build files

	```sh
	cd wxFormBuilder
	git submodule init
	git submodule update
	./create_build_files4.sh
	```
4. Build

	```sh
	cd build/3.0/gmake
	make config=release
	```
4. Test

	```sh
	cd ../../../output/bin/
	./wxformbuilder
	```
