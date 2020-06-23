package(default_visibility = ["//visibility:public"])
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

cc_library(
    name = "proto_value",
    srcs = ["protovalue.cc"],
    hdrs = ["protovalue.h"],
    deps = ["@com_google_gtest//:gtest",
            "@com_google_absl//absl/types:variant",
            "@com_google_protobuf//:protobuf"],
)

cc_test(
    name = "protovalue_test",
    srcs = ["protovalue_test.cc"],
    deps = [
        "proto_value",
    ],
)
