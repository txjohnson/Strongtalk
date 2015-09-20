BUILD_DIR	= "build"
VM_SRC_DIR = "vm"
INCL_PATH	= ["-I"+BUILD_DIR, "-I"+VM_SRC_DIR+"/asm", "-I"+VM_SRC_DIR+"/code", "-I"+VM_SRC_DIR+"/compiler", "-I"+VM_SRC_DIR+"/disasm", "-I"+VM_SRC_DIR+"/interpreter", "-I"+VM_SRC_DIR+"/lookup", "-I"+VM_SRC_DIR+"/memory", "-I"+VM_SRC_DIR+"/oops", "-I"+VM_SRC_DIR+"/prims", "-I"+VM_SRC_DIR+"/recompiler", "-I"+VM_SRC_DIR+"/runtime", "-I"+VM_SRC_DIR+"/topIncludes", "-I"+VM_SRC_DIR+"/utilities"]

TEST_SRC_DIR 	= "test"
TEST_INCL_PATH = ["-I"+TEST_SRC_DIR+"/runtime", "-I"+TEST_SRC_DIR+"/utilities", "-I"+TEST_SRC_DIR+"/memory"]

DEFINES = ["-DDELTA_COMPILER", "-DASSERT", "-DDEBUG", "-D__LINUX__"]
COPTS = ["-m32", "-fno-rtti", "-Wno-write-strings", "-fno-operator-names", "-fms-extensions"]

#https://groups.google.com/forum/#!topic/bazel-discuss/8Snn9hLuSqo

cc_library(
    name = "easyunit",
    srcs = glob(["easyunit/src/*.cpp"]),
    hdrs = glob(["easyunit/easyunit/*.h"]),
    copts = ["-m32"],
    linkopts = ["-m32"],
    includes = ["easyunit/easyunit"],
)

cc_library(
    name = "libstrongtalk",
    srcs = glob(["vm/**/*.cpp"], exclude=["vm/runtime/main.cpp"]),
    copts = INCL_PATH + COPTS + DEFINES,
    linkopts = ["-m32", "-lpthread", "-ldl", "-lrt", "-lm"],
)

cc_binary(
    name = "strongtalk",
    srcs = ["vm/runtime/main.cpp"],
    copts = INCL_PATH + COPTS + DEFINES,
    #linkstatic=0,
    deps = [":libstrongtalk"],
    
    data = ["strongtalk.bst"],
)

cc_test(
    name = "strongtalk-test",
    srcs = glob(["test/**/*.cpp"], exclude=[
        "test/compiler/compiler_tests.cpp",
        "test/interpreter/missingMethodBuilderTest.cpp",
        "test/memory/contextKlassTests.cpp",
        "test/memory/methodLookupTests.cpp",
        "test/memory/proxyPrimsTest.cpp",
        "test/prims/alienIntegerCallout0Tests.cpp",
        "test/prims/alienIntegerCallout1Tests.cpp",
        "test/prims/alienIntegerCallout2Tests.cpp",
        "test/prims/alienIntegerCallout*Tests.cpp",    # up to 7
        "test/prims/byteArrayPrimsTests.cpp",
        "test/prims/indirectAlienPrimsTests.cpp",
        "test/prims/pointerAlienPrimsTest.cpp",
    ]),
    copts = INCL_PATH + TEST_INCL_PATH + COPTS + DEFINES,
    #linkstatic=0,
    deps = [":libstrongtalk", ":easyunit"],
    
    data = ["strongtalk.bst"],
    args = ["-b strongtalk.bst"],
    size = "small",
)