package(default_visibility = ["//visibility:public"])

cc_library(
    name = "request_handler_lib",
    srcs = [
        "request_handler.cc",
    ],
    hdrs = ["request_handler.h"],
    deps = [
        "@pistache//:pistache_lib",
        "@github_nlohmann_json//:json",
        "//fetcher/proto:hologram_config_cc_proto", 
        "//fetcher/proto:hologram_availability_cc_proto",
        "@com_google_absl//absl/strings"
    ],
    data = glob([
        "sample_proto/Chipper/*.textproto",
        "sample_proto/Chipper_GDPR/*.textproto",
        "sample_proto/hologram_config.ascii"
    ])
)

cc_test(
    name = "request_handler_test",
    srcs = ["request_handler_test.cc"],
    deps = [
        "request_handler_lib",
        "@com_google_gtest//:gtest",
        "@pistache//:pistache_lib"
    ]
)

filegroup(
    name = "srcs",
    srcs = ["BUILD"] + glob(["*.cc"]),
)