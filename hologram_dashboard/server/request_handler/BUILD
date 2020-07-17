package(default_visibility = ["//visibility:public"])

cc_library(
    name = "request_handler_lib",
    srcs = [
        "request_handler.cc",
    ],
    hdrs = ["request_handler.h"],
    deps = [
        "@pistache//:pistache_lib",
        "@github_nlohmann_json//:json"
    ],
)

filegroup(
    name = "srcs",
    srcs = ["BUILD"] + glob(["*.cc"]),
)