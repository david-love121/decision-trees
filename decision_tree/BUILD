load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "decision_tree_lib",
    srcs = [
        "decision_tree.hpp",
        "node.hpp"
    ]
)
cc_binary(
    name = "decision_tree",
    srcs = [
        "main.cpp",
    ],
    deps = [
        ":decision_tree_lib",
        "//dataset:dataset",
        "@googletest//:gtest_main"
    ]
)