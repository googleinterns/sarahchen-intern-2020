cc_binary(
  name = "demo_generator",
  srcs = ["demo_sample_generator_main.cc"],
  deps = [
    "//proto:event_predicate_cc_proto",
    "//src:proto_value",
    "//src:proto_parser"
  ],
  data = glob(["proto_texts/*.txt"])
)

cc_binary(
  name = "demo",
  srcs = ["demo.cc"],
  deps = [
    "//proto:event_predicate_cc_proto",
    "//src:proto_value",
  ],
  data = glob(["proto_texts/*.txt"])
)