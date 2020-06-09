package(default_visibility = ["//visibility:public"])
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "enums_proto",
    srcs = ["enums.proto"],
)

proto_library(
    name = "hologram_availability_proto",
    srcs = ["hologram_availability.proto"],
    deps = [":enums_proto"]
)

cc_proto_library(
    name = "hologram_availability_cc_proto",
    deps = [":hologram_availability_proto"],
)

proto_library(
    name = "hologram_config_proto",
    srcs = ["hologram_config.proto"],
    deps = [":enums_proto"]
)

cc_proto_library(
    name = "hologram_config_cc_proto",
    deps = [":hologram_config_proto"],
)

cc_library(
    name = "fetcher_lib",
    srcs = ["hologram_availability_fetcher.cc"],
    hdrs = ["hologram_availability_fetcher.h"],
    deps = [":hologram_availability_cc_proto", ":hologram_config_cc_proto", 
    "@com_github_gflags_gflags//:gflags"],
)