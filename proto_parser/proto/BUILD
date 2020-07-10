package(default_visibility = ["//visibility:public"])
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "example_proto",
    srcs = ["example.proto"],
)

cc_proto_library(
    name = "example_cc_proto",
    deps = [":example_proto"],
)

proto_library(
    name = "event_predicate_proto",
    srcs = ["event_predicate.proto"],
)

cc_proto_library(
    name = "event_predicate_cc_proto",
    deps = [":event_predicate_proto"],
)