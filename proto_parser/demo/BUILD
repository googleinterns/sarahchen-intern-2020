cc_binary(
  name = "demo_generator",
  srcs = ["demo_generator.cc"],
  deps = [
    "//proto:event_predicate_cc_proto",
    "//src:proto_value",
    "//src:proto_parser"
  ],
  data = glob(["proto_texts/*.txt"])
)

cc_binary(
  name = "demo_main",
  srcs = ["demo_main.cc"],
  deps = [
    "//proto:event_predicate_cc_proto",
    "//src:proto_value",
    "//src:proto_parser"
  ],
  data = glob(["proto_texts/*.txt"])
)