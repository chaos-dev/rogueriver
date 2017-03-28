# Rogue River: Obol of Charon
Explore five procedurally generated rivers as you fight to reclaim your loved one from the Greek Underworld.

![screenshot](Screenshot.png?raw=true)

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3faceb296121471b8dd59b0927c0c62e)](https://www.codacy.com/app/chaos-dev/rogueriver?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=chaos-dev/rogueriver&amp;utm_campaign=Badge_Grade)
| **Linux/OSX**: [![Build Status](https://travis-ci.org/chaos-dev/rogueriver.svg)](https://travis-ci.org/chaos-dev/rogueriver)
| **Windows**: [![Build status](https://ci.appveyor.com/api/projects/status/pxr3eg4fyw70s0am/branch/master?svg=true)](https://ci.appveyor.com/project/chaos-dev/rogueriver/branch/master)

This roguelike game was created for the 2017 Seven-Day Roguelike Challenge. It uses [BearLibTerminal](http://foo.wyrd.name/en:bearlibterminal) as the graphical front-end.

## Downloading
 
You can downloads Windows, OSX, and Linux releases from [the Github Releases page](https://github.com/chaos-dev/rogueriver/releases).

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

Please report any problems on the [Github Issues page](https://github.com/chaos-dev/rogueriver/issues).

#### Will there be further development or versions of this game?

This game was created for the 2017 7-Day Roguelike Challenge, and was completed
for that challenge.  Expanding it would go against the spirit of the challenge.
The current plan is to only release fixes, but not new features or a 2.0.0 version.
