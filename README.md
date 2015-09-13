# Strongtalk

Smalltalk… with a need for speed

## Badges

drone.io (makefile): [![Build Status](https://drone.io/github.com/jirkadanek/Strongtalk/status.png)](https://drone.io/github.com/jirkadanek/Strongtalk/latest)      travis-cl (cmake): TBD      Coverity (cmake): [![Coverity Scan Build Status](https://scan.coverity.com/projects/6340/badge.svg)](https://scan.coverity.com/projects/jirkadanek-strongtalk)

## About this fork / Roadmap

*Mosty small fixes all around, nothing ground breaking or particularly useful to anybody. It compiles and that's pretty much it.*

This fork fixes the Linux makefile () and some coding errors () so that the project now builds on Linux. In addition, it updates the code so that legacy compiler flags (like `-fpermissive` () and `-fno-for-scope` ()) can be dropped. As a side effect, this makes the project compile with clang, as well as GCC. Next, build configuration files for cmake () and Bazel () were added. Some errors found with the `scan.coverity.com` tool were fixed (, , , ) in hopes it would help debugging mysterious crashes in the `strongtalk-test` binary (and in the vm itself, if one runs it with valgrind or Address Sanitizer). These hopes proved futile so far. Furthermore, the code was refactored a bit where it makes obvious sense ().

The repository contains binaries. Keeping the Strongtalk image (strongtalk.bst) in git makes some sense, all other binaries must be removed, eventually.

## Building on Linux

*This fork was tested only on x86_64 Arch Linux. It will likely work on other distributions. Strongtalk's JIT generates machine code, so IA-32 (or the backwards-compatible x86_64) CPU is required. See README.txt or wiki pages on Google Code for build instructions on WIndows and OSX.*

### make (primary)

In the `build` directory, run

    make -f ../build.linux/makefile ROOT_DIR=$(cd ..; pwd) default
    
This performs in-source build. Binaries are placed in the `build` directory.
    
### cmake

TBD

The resurrected [Self language](https://github.com/russellallen/self) builds with cmake too. This should be primary option in the future.

### Bazel

TBD

Resulting binary is, for some reason, one third the size of the cmake one. Too bad Bazel is an unwieldy dependency.

## Running it

*Linux support is minimal. It is only possible to run scripts from the command line (with `-script`). The GUI stuff (meaning Strongtalk IDE) work only on WIndows.*

Check that the binary works by running ` tools/test3.dlt`, a simple test script. It should not crash and it should print `false`.

    build/strongtalk -script tools/test3.dlt
    
Try also

    build/strongtalk -script tools/test3.dlt

output of this should end with

    Time = 1198
    Testing completed.
    identifier expected

    Halt: 
    Process state: Stopped
    - Stack trace (1, 40)
    [...]

and then it ?crashes? and opens a REPL of sorts. Try evaluating `^3+4`.

## About Smalltalk

Smalltalk can be called a Lisp of the Object Oriented world. The language is fairly minimal and as much functionality as possible is pushed into the standard library. That includes program flow constructs, like if-then-else. All code in the standard library is visible to the programmer and can be easily modified and extended. Smalltalk is similar to Ruby in this respect. Or Emacs.

Good introduction to Smalltalk in Czech is a series of articles [Squeak: návrat do budoucnosti](http://www.root.cz/clanky/squeak-navrat-do-budoucnosti/) by Pavel Křivánek.

## About Strongtalk

Strongtalk is a legendary virtual machine and Smalltalk implementation created from 1995 to 1996 by Longview Technologies, a small startup company. Strongtalk's main features were optional sound type system and the groundbreaking use of JIT compilation. In 1997, Longview Technologies were acquired by Sun Microsystems and the team working on Strongtalk was put to work on the Java HotSpot VM. All work on Strongtalk ceased in favor of Java and Strongtalk has never become a product.

In 2006, Sun released Strongtalk under the BSD licence. David Greenwald, CEO of the original startup, became leader of the developer community around it. See [http://strongtalk.org/](http://strongtalk.org/) for more on the history of Strongtalk.

Strongtalk was again abandoned in 2011. See the mailing list [strongtalk-general](https://groups.google.com/forum/#!forum/strongtalk-general) to get the Zeitgeist.