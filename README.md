WXFORMBUILDER INSTALL FROM REPOSITORIES
---------------------------------------

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

WXFORMBUILDER INSTALL FROM SOURCE
---------------------------------

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


