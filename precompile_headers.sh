pushd benchmarkrepo/build
    make -f ../build.linux/makefile ROOT_DIR=$(cd ..; pwd) dependencies
    time make  -f ../build.linux/makefile ROOT_DIR=$(cd ..; pwd) incls/_precompiled.incl.gch
popd