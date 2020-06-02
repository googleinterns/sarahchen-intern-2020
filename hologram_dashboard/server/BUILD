package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "server",
    srcs = ["server.cc"],
    deps = [
        "//server/request_handler:request_handler_lib"
    ],
)

load("@io_bazel_rules_docker//cc:image.bzl", "cc_image")

cc_image(
    name = "server_image",
    base = "@cc_base_image//image",
    binary = ":server",
)

filegroup(
    name = "srcs",
    srcs = ["BUILD"] + glob(["*.cc"]),
)