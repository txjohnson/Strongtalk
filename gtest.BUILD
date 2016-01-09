cc_library(
    name = "main",
    srcs = glob(
        ["src/*.cc"],
        exclude = ["src/gtest-all.cc"]
    ),
    hdrs = glob([
        "include/**/*.h",
        "src/*.h"
    ]),
    copts = ["-O", "-m32", "-Iexternal/gtest/include"],
    linkopts = ["-m32", "-pthread"],
    visibility = ["//visibility:public"],
)
