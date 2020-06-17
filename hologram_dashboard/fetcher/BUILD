package(default_visibility = ["//visibility:public"])

cc_library(
    name = "hologram_data_fetcher_lib",
    hdrs = ["hologram_data_fetcher.h"]
)

cc_library(
    name = "flags",
    srcs = ["flags.cc"],
    hdrs = ["flags.h"],
    deps = ["@com_google_absl//absl"]
)

cc_library(
    name = "data_availability_fetcher",
    srcs = ["hologram_data_source_availability_fetcher.cc"],
    hdrs = ["hologram_data_source_availability_fetcher.h"],
    deps = ["//fetcher/proto:hologram_availability_cc_proto", 
            "//fetcher/proto:hologram_config_cc_proto", 
            "@com_google_gtest//:gtest",
            "hologram_data_fetcher_lib",
            "flags"],
)

cc_test(
    name = "data_availability_fetcher_test",
    srcs = ["hologram_data_source_availability_fetcher_test.cc"],
    deps = [
        "data_availability_fetcher",
    ],
    data = ["testdata/hologram_config_valid.ascii", 
            "testdata/hologram_config_malformed.ascii"]
)

cc_binary(
    name = "fetcher_main",
    srcs = ["fetcher_main.cc"],
    deps = [
        "data_availability_fetcher"
    ],
)