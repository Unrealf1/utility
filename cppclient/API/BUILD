load("@rules_foreign_cc//foreign_cc:defs.bzl", "configure_make")
load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

configure_make(
    name = "zmq",
    autoconf=True,
    lib_source= "@zmq//:all",
    out_shared_libs=["libzmq.so"],
    visibility = ["//visibility:public"],
)

cmake(
    name = "zmqpp",
    lib_source = "@zmqpp//:all",
    out_shared_libs=["libzmqpp.so"],
    visibility = ["//visibility:public"],
    deps = ["zmq"],
    cache_entries = {"ZEROMQ_INCLUDE_DIR":"/root/.cache/bazel/_bazel_root/5d9fc5e043decb405ef49726c05a1be7/execroot/__main__/bazel-out/k8-fastbuild/bin/API/copy_zmq/zmq/include/", "ZEROMQ_LIB_DIR":"/root/.cache/bazel/_bazel_root/5d9fc5e043decb405ef49726c05a1be7/execroot/__main__/bazel-out/k8-fastbuild/bin/API/copy_zmq/zmq/lib/"}
)

cmake(
    name = "spdlog",
    lib_source = "@spdlog//:all",
    out_static_libs=["libspdlog.a"],
    visibility = ["//visibility:public"],
    
)


cc_library(
    name = "CommunicatorApi",
    includes = ["include"],
    srcs = [
        "source/Communicator.cpp", 
        "source/Endpoint.cpp", 
        "source/Identities.cpp", 
        "source/ZMQTMWorker.cpp", 
        "source/ZMQTransportManager.cpp"
    ] + glob(["include/**/*.hpp"]),
    copts = ["-std=c++17", "-lpthread"],
    visibility = ["//visibility:public"],
    
    deps = ["zmq", "zmqpp", "spdlog"],
    linkopts = ["-pthread"],
)
