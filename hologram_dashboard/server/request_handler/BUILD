package(default_visibility = ["//visibility:public"])

cc_library(
    name = "flags_lib",
    srcs = ["flags.cc"],
    hdrs = ["flags.h"],
    deps = ["@com_google_absl//absl/flags:flag"]
)

cc_library(
    name = "hologram_data_availability_reader_lib",
    srcs = [
        "hologram_data_availability_reader.cc"
    ],
    hdrs = [
        "hologram_data_availability_reader.h"
    ],
    deps = [
        "@github_nlohmann_json//:json",
        "//fetcher/proto:hologram_config_cc_proto", 
        "//fetcher/proto:hologram_availability_cc_proto",
        "@com_google_absl//absl/strings"
    ],
)

cc_test(
    name = "hologram_data_availability_reader_test",
    srcs = ["hologram_data_availability_reader_test.cc"],
    deps = [
        "hologram_data_availability_reader_lib",
        "@com_google_gtest//:gtest"
    ],
    data = ["sample_proto/Test/Chipper/SPAM.textproto",
            "sample_proto/Test/hologram_config.ascii"]
)

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
        "@com_google_absl//absl/strings",
        "hologram_data_availability_reader_lib",
        "flags_lib"
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
    ],
    data = ["sample_proto/Test/Chipper/SPAM.textproto",
            "sample_proto/Test/hologram_config.ascii"]
)

filegroup(
    name = "srcs",
    srcs = ["BUILD"] + glob(["*.cc"]),
)