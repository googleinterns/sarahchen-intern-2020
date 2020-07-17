package(default_visibility = ["//visibility:public"])
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

cc_library(
    name = "proto_value",
    srcs = ["proto_value.cc"],
    hdrs = ["proto_value.h"],
    deps = ["@com_google_gtest//:gtest",
            "@com_google_absl//absl/types:variant",
            "@com_google_absl//absl/strings",
            "@com_google_absl//absl/container:flat_hash_map",
            "@com_google_protobuf//:protobuf"],
)


cc_library(
    name = "message_value",
    hdrs = ["message_value.h"],
    srcs = ["message_value.cc"],
    deps = ["proto_value"]
)

cc_library(
    name = "primitive_value",
    hdrs = ["primitive_value.h"],
    srcs = ["primitive_value.cc"],
    deps = ["proto_value"]
)

cc_test(
    name = "primitive_value_test",
    srcs = ["primitive_value_test.cc"],
    deps = [
        "primitive_value",
        "//proto:event_predicate_cc_proto"
    ],
)

cc_library(
    name = "proto_parser",
    srcs = ["proto_parser.cc"],
    hdrs = ["proto_parser.h"],
    deps = ["proto_value",
            "primitive_value",
            "message_value"]
)

cc_test(
    name = "proto_parser_test",
    srcs = ["proto_parser_test.cc"],
    deps = [
        "proto_parser", 
        "proto_value",
        "//proto:example_cc_proto"
    ],
)
