cc_binary(
    name = "client",
    srcs = ["client/source/main.cpp"],
    deps = [
        "//API:CommunicatorApi",
    ],
    copts = ["-std=c++17"],
)

cc_binary(
    name = "server",
    srcs = ["server/source/main.cpp"],
    deps = [
        "//API:CommunicatorApi",
    ],
    copts = ["-std=c++17"],
)
