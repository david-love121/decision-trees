load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "decision_tree",
    srcs = [
        "main.cpp",
        "decision_tree.hpp",
        "node.hpp",
    ],
    deps = [
        "//data_container:data_container",
        "@googletest//:gtest_main",
    ],
)