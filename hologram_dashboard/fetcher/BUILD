package(default_visibility = ["//visibility:public"])

cc_library(
    name = "hologram_data_fetcher_lib",
    hdrs = ["hologram_data_fetcher.h"]
)

cc_library(
    name = "hologram_data_source_availabiliity_fetcher",
    srcs = ["hologram_data_source_availabiliity_fetcher.cc"],
    hdrs = ["hologram_data_source_availability_fetcher.h"],
    deps = ["//fetcher/proto:hologram_availability_cc_proto", 
            "//fetcher/proto:hologram_config_cc_proto", 
            "@com_github_gflags_gflags//:gflags",
            "hologram_data_fetcher_lib"],
)

cc_binary(
    name = "fetcher_main",
    srcs = ["fetcher_main.cc"],
    deps = [
        "hologram_data_source_availabiliity_fetcher"
    ],
)