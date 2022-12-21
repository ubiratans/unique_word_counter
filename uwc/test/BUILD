load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_test")

cc_test(
    name = "uwc_test",
    srcs = glob(
        [
            "*.h",
            "*.cpp",
            "mock/*.h",
        ],
    ),
    copts = ["-std=c++17"],
    deps = [
        "//uwc",
        "@com_google_googletest//:gtest_main",
        "@com_google_googletest//:gtest",
    ],
)
