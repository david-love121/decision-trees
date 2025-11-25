load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "decision_tree_lib",
    hdrs = [
        "decision_tree.hpp",
        "node.hpp"
    ],
    deps = [
        "//dataset:dataset",
        "//data_container:data_container",
    ],
    visibility = ["//visibility:public"],
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