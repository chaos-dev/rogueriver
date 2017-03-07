# Rogue River: Obol of Charon
A roguelike where you boat down the 5 rivers of the Greek Underworld and battle mythical monsters.

**Linux/OSX**: [![Build Status](https://travis-ci.org/chaos-dev/rogueriver.svg)](https://travis-ci.org/chaos-dev/rogueriver)

**Windows**: [![Build status](https://ci.appveyor.com/api/projects/status/pxr3eg4fyw70s0am/branch/master?svg=true)](https://ci.appveyor.com/project/chaos-dev/rogueriver/branch/master)

## Compiling from Source

Windows and Linux platforms, both 32 and 64 bit, are supported.  Mac (OSX)
builds and executable are provided, but they are not supported.  They are
only provided as a courtesy; OSX specific bugs may or may not be fixed.

In order to compile from source, you will need CMake version 2.8 or
higher.  _This is the only supported build system_.  You must also have
a C compiler that supports c++14.

For Linux, you can use the following example from the root of the source
directory to build the program:

```
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release
make
```

For Windows, you can use the following example from the root of the source
directory to build the program.  You may need to change the "Visual
Studio" specification to match your compiler.

```
md build
cd build
cmake -G"Visual Studio 14 2015 Win64" ..
cmake --build . --config Release
```

## Frequently Asked Questions

#### I found a bug. What do I do?

Please report any issues on the Github Issues page.
