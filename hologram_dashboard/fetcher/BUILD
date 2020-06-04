load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "hologram_availability_proto",
    srcs = ["hologram_availability.proto"],
    deps = ["@com_google_protobuf//:timestamp_proto"],
)

cc_proto_library(
    name = "hologram_availability_cc_proto",
    deps = [":hologram_availability_proto"],
)

cc_binary(
    name = "fetcher",
    srcs = ["fetcher.cc"],
    deps = [":hologram_availability_cc_proto"],
)