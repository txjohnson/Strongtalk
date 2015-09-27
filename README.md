## Conclusion

Regardless of build system, precompiling headers takes 0.5s, building serially without precompiled headers takes about 84s, serially with precompiled headers 38s, in parallel without precompiled headers 23s and in parallel with precompiled headers 10s. Using precompiled headers does not seem to have significant effect on the binary sizes. (I tested this with GCC)

Experiment with https://github.com/sakra/cotire

    wget https://raw.githubusercontent.com/sakra/cotire/master/CMake/cotire.cmake
    # modify CMakeLists.txt
    mkdir benchmarkrepo/benchmarkbuild
    pushd benchmarkrepo/benchmarkbuild
        cmake -DCMAKE_BUILD_TYPE=Debug ..  # -G Ninja
        time make # ninja
    popd

This fails with "redefinition/previous definition of", probably because headers do not have include guards.
    
Combining ccache with Bazel is s waste of effort, ccache speads up clean builds, Bazel is here to avoid clean builds. If I plan to use Bazel as a development build sysem (which I do), ccache is useless.

Makefile does not support compiling in parallel and does not recompile headers if some of the sources change.

I've decided to stop using (and updating and everything) the Makefiles. I'll either focus on making CMake and Bazel rules complete (that means makedeps), or possibly drop makedeps and add explicit includes. But this is a busywork and not fun. first I want to focus on the new interpreter and only then possibly do this. Removing makedeps would simplify the build. One less thing to worry about. Using makedeps for platform independence is IMO not a sufficient justification for its existence.

## Measurements

sh clone_repo.sh
sh precompile_headers.sh
# do one of the tests below
sh delete_repo.sh

write down the real (wall clock) time

## precompile_headers.sh

0m0.526s
0m0.506s
0m0.506s
0m0.509s
0m0.512s
(stopped writing it down at this point)

## Makefiles

### gch

    pushd benchmarkrepo/build
        time make  -f ../build.linux/makefile ROOT_DIR=$(cd ..; pwd) objects
        time make  -f ../build.linux/makefile ROOT_DIR=$(cd ..; pwd) link
    popd
    
0m37.704s, 0m0.909s
0m37.577s, 0m0.834s

sizes:

$ ls -AlF benchmarkrepo/build/strongtalk benchmarkrepo/build/strongtalk-test benchmarkrepo/build/strongtalk.so 
-rwxr-xr-x 1 jirka jirka    48604 Sep 26 13:03 benchmarkrepo/build/strongtalk*
-rwxr-xr-x 1 jirka jirka   143264 Sep 26 13:03 benchmarkrepo/build/strongtalk-test*
-rwxr-xr-x 1 jirka jirka 13419684 Sep 26 13:03 benchmarkrepo/build/strongtalk.so*


### gch, -j8

    pushd benchmarkrepo/build
        time make  -f ../build.linux/makefile ROOT_DIR=$(cd ..; pwd) -j8 objects
        time make  -f ../build.linux/makefile ROOT_DIR=$(cd ..; pwd) -j8 link
    popd

0m37.803s, 0m1.039s

### -j8
    
    pushd benchmarkrepo/build
        echo > incls/_precompiled.incl.gch # deleting does not work, make would recreate it
        time make  -f ../build.linux/makefile ROOT_DIR=$(cd ..; pwd) -j8 objects
        time make  -f ../build.linux/makefile ROOT_DIR=$(cd ..; pwd) -j8 link
    popd
    
1m23.277s, 0m0.928s

### (nothing)

    pushd benchmarkrepo/build
        echo > incls/_precompiled.incl.gch # deleting does not work, make would recreate it
        time make  -f ../build.linux/makefile ROOT_DIR=$(cd ..; pwd) objects
        time make  -f ../build.linux/makefile ROOT_DIR=$(cd ..; pwd) link
    popd

1m23.458s, 0m1.040s
1m24.121s, 0m1.005s

sizes:

-rwxr-xr-x 1 jirka jirka    48604 Sep 26 13:07 benchmarkrepo/build/strongtalk*
-rwxr-xr-x 1 jirka jirka   143264 Sep 26 13:07 benchmarkrepo/build/strongtalk-test*
-rwxr-xr-x 1 jirka jirka 13419920 Sep 26 13:07 benchmarkrepo/build/strongtalk.so
    
# CMake

## (nothing)

    rm benchmarkrepo/build/incls/_precompiled.incl.gch # cmake will not recreate it
    mkdir benchmarkrepo/benchmarkbuild
    pushd benchmarkrepo/benchmarkbuild
        cmake -DCMAKE_BUILD_TYPE=Debug ..  # -G Ninja
        time make # ninja
    popd
    
1m35.120s
1m34.246s

$ ls -AlF benchmarkrepo/benchmarkbuild/strongtalk benchmarkrepo/benchmarkbuild/strongtalk-test
-rwxr-xr-x 1 jirka jirka 12510800 Sep 26 13:41 benchmarkrepo/benchmarkbuild/strongtalk*
-rwxr-xr-x 1 jirka jirka 17338448 Sep 26 13:41 benchmarkrepo/benchmarkbuild/strongtalk-test*

### ghc

    pushd benchmarkrepo/build # precompile headers w/ options CMake uses
        g++ -c -x c++-header -o /home/jirka/Bin/benchmark/benchmarkrepo/build/incls/_precompiled.incl.gch -DASSERT -DDEBUG -DDELTA_COMPILER -m32 -fno-rtti -Wno-write-strings -fno-operator-names -fms-extensions -ffriend-injection -g -I/home/jirka/Bin/benchmark/benchmarkrepo/build -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/asm -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/code -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/compiler -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/disasm -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/interpreter -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/lookup -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/memory -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/oops -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/prims -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/recompiler -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/runtime -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/topIncludes -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/utilities -I/home/jirka/Bin/benchmark/benchmarkrepo/easyunit -I/home/jirka/Bin/benchmark/benchmarkrepo/easyunit/easyunit -I/home/jirka/Bin/benchmark/benchmarkrepo/test/runtime -I/home/jirka/Bin/benchmark/benchmarkrepo/test/utilities -I/home/jirka/Bin/benchmark/benchmarkrepo/test/memory incls/_precompiled.incl
    popd
    mkdir benchmarkrepo/benchmarkbuild
    pushd benchmarkrepo/benchmarkbuild
        cmake -DCMAKE_BUILD_TYPE=Debug ..  # -G Ninja
        time make # ninja
    popd
    

    
1m34.987s ## different options (defines, even) when precompiling header
0m44.025s



### ghc -j8
    pushd benchmarkrepo/build # precompile headers w/ options CMake uses
        g++ -c -x c++-header -o /home/jirka/Bin/benchmark/benchmarkrepo/build/incls/_precompiled.incl.gch -DASSERT -DDEBUG -DDELTA_COMPILER -m32 -fno-rtti -Wno-write-strings -fno-operator-names -fms-extensions -ffriend-injection -g -I/home/jirka/Bin/benchmark/benchmarkrepo/build -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/asm -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/code -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/compiler -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/disasm -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/interpreter -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/lookup -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/memory -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/oops -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/prims -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/recompiler -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/runtime -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/topIncludes -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/utilities -I/home/jirka/Bin/benchmark/benchmarkrepo/easyunit -I/home/jirka/Bin/benchmark/benchmarkrepo/easyunit/easyunit -I/home/jirka/Bin/benchmark/benchmarkrepo/test/runtime -I/home/jirka/Bin/benchmark/benchmarkrepo/test/utilities -I/home/jirka/Bin/benchmark/benchmarkrepo/test/memory incls/_precompiled.incl
    popd
    mkdir benchmarkrepo/benchmarkbuild
    pushd benchmarkrepo/benchmarkbuild
        cmake -DCMAKE_BUILD_TYPE=Debug ..  # -G Ninja
        time make -j8 # ninja
    popd
    
0m9.747s

### ghc, Ninja
    pushd benchmarkrepo/build # precompile headers w/ options CMake uses
        g++ -c -x c++-header -o /home/jirka/Bin/benchmark/benchmarkrepo/build/incls/_precompiled.incl.gch -DASSERT -DDEBUG -DDELTA_COMPILER -m32 -fno-rtti -Wno-write-strings -fno-operator-names -fms-extensions -ffriend-injection -g -I/home/jirka/Bin/benchmark/benchmarkrepo/build -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/asm -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/code -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/compiler -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/disasm -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/interpreter -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/lookup -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/memory -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/oops -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/prims -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/recompiler -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/runtime -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/topIncludes -I/home/jirka/Bin/benchmark/benchmarkrepo/vm/utilities -I/home/jirka/Bin/benchmark/benchmarkrepo/easyunit -I/home/jirka/Bin/benchmark/benchmarkrepo/easyunit/easyunit -I/home/jirka/Bin/benchmark/benchmarkrepo/test/runtime -I/home/jirka/Bin/benchmark/benchmarkrepo/test/utilities -I/home/jirka/Bin/benchmark/benchmarkrepo/test/memory incls/_precompiled.incl
    popd
    mkdir benchmarkrepo/benchmarkbuild
    pushd benchmarkrepo/benchmarkbuild
        cmake -DCMAKE_BUILD_TYPE=Debug ..  -G Ninja
        time ninja
    popd
    
0m9.038s 

$ ls -AlF benchmarkrepo/benchmarkbuild/strongtalk benchmarkrepo/benchmarkbuild/strongtalk-test 
-rwxr-xr-x 1 jirka jirka 12500140 Sep 26 13:31 benchmarkrepo/benchmarkbuild/strongtalk*
-rwxr-xr-x 1 jirka jirka 17321388 Sep 26 13:31 benchmarkrepo/benchmarkbuild/strongtalk-test*

# Ninja

    mkdir benchmarkrepo/benchmarkbuild
    pushd benchmarkrepo/benchmarkbuild
        cmake -DCMAKE_BUILD_TYPE=Debug ..  -G Ninja
        time ninja
    popd

this runs with -j8 on its own

0m22.559s # w different options on precompiled header


# bazel and defaults, which includes -j8 imo (does not use precompiled headers)

    rm benchmarkrepo/build/incls/_precompiled.incl.gch
    pushd benchmarkrepo
        time ~/bin/bazel # this installs bazel into the workspace
        time ~/bin/bazel build //:all # bazel produces its own timing information in addition to this
    popd
    
0m15.182s
0m10.471s
0m2.237s # I suspect some sneaky caching

    rm -rf /home/jirka/.cache/bazel
    
0m22.290s
0m21.952s # that's more reasonable

