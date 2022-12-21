cc_library(
    name = "uwc",
    hdrs = glob(["include/*.h", "include/worker/*.h"]),
    srcs = glob(["src/*.cpp"]),
    copts = ["-std=c++17"],
    linkopts = ["-pthread"],
    visibility = [
        "//:__subpackages__",
    ],
    deps = [
    ],
)
