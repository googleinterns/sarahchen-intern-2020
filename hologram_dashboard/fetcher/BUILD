package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "fetcher",
    srcs = ["fetcher.cc"],
    deps = [
        "//fetcher/fetcher_lib:fetcher_lib"
    ],
)

cc_library(
    name = "fetcher_lib",
    srcs = ["hologram_availability_fetcher.cc"],
    hdrs = ["hologram_availability_fetcher.h"],
    deps = [":hologram_availability_cc_proto", ":hologram_config_cc_proto", 
    "@com_github_gflags_gflags//:gflags"],
)