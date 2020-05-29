package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "hda_server",
    srcs = ["server.cpp"],
    deps = [
        "//main/hda:hda_lib"
    ],
)

load("@io_bazel_rules_docker//cc:image.bzl", "cc_image")

cc_image(
    name = "server",
    base = "@cc_base_image//image",
    binary = ":hda_server",
)

filegroup(
    name = "srcs",
    srcs = ["BUILD"] + glob(["*.cc"]),
)