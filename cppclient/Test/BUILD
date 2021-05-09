cc_binary(
    name = "test",
    srcs = glob(["source/*.cpp"]) + glob(["include/*.hpp"]),
    includes = ["include"],
    deps = [
        "//API:CommunicatorApi",
        "@gtest//:gtest_main"
    ],
    copts = ["-std=c++17"],
    linkopts = ["-pthread"],
)