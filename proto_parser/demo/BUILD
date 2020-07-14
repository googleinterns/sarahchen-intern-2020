cc_binary(
  name = "demo_sample_generator_main",
  srcs = ["demo_sample_generator_main.cc"],
  deps = [
    "//proto:event_predicate_cc_proto",
    "//src:proto_value",
    "//src:proto_parser"
  ],
  data = glob(["proto_texts/*.textproto"])
)