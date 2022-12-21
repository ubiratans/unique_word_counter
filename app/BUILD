load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "unique_word_counter",
    srcs = ["main.cpp"],
    copts = ["-std=c++17"],
    # Set this variable to the right path for llvm-symbolizer on your machine.
    deps = [
      "//uwc",
    ],
)
