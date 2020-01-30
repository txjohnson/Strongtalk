# Strongtalk

Smalltalk… with static types.


## About this fork
Nothing to see here. Fixes to get Strongtalk working on MacOS. Strongtalk uses 32bit tagged pointers everywhere. It will be sometime before this fork will actually run -- it compiles though. So that that's good. There are thousands of warnings. At a glance, there are many for string literals being passed around as plain char pointers. There are more for stdcalls in the compiler... and more...

## Building on MacOS
Open the Xcode project in build.osx. Enjoy.

## Building for others
See jirkadanek's notes on building for Linux and Windows. No guarantee that this fork will work, though.

## What to Expect
Right now, massive breakage. Hopefully, we'll see a clean build under C++17 or later.