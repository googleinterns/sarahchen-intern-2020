package(default_visibility = ["//visibility:public"])

cc_library(
    name = "hda_lib",
    srcs = [
        "hda.cpp",
    ],
    hdrs = ["hda.h"],
    deps = [
        "@pistache//:pistache_lib",
    ],
)

filegroup(
    name = "srcs",
    srcs = ["BUILD"] + glob(["*.cc"]),
)