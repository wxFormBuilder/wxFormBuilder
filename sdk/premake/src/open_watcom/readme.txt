Open Watcom support added July 2008
Tim Channon, tc@gpsl.net

Targets Code:Blocks
Open Watcom 1.7a 

Instructions on enabling debugging are on the Open Watcom wiki.
Essentially changing the OW configuration of C::B slightly and calling the 
OW Windows or text windowed debugger.

Switch the toolchain from mingwin32-gcc to OW or back is as trivial as

premake --target cb-gcc
premake --target cb-ow

The OW C++ compiler is being upgraded, with 1.8 already up to 2008 
standards in some areas, others are in progress.

A toolchain difference which might be noticed is a build speed 
difference.

OW can target other platforms. is used for embedded DOS work as well.
There are interesting possibilities with Linux, OW can generate Linux 
executables, so it would be interesting to know what C::B under Linux
and OW could do if anything.

Whether premake will ever target OW wmake is undecided. Cross platform 
makes can be extremely complex.