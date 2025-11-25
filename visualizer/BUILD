load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

genrule(
    name = "moc_main_window",
    srcs = ["main_window.hpp"],
    outs = ["moc_main_window.cpp"],
    cmd = "/usr/lib/qt6/moc $(location main_window.hpp) -o $@",
    tags = ["local"],
)

genrule(
    name = "moc_tree_scene",
    srcs = ["tree_scene.hpp"],
    outs = ["moc_tree_scene.cpp"],
    cmd = "/usr/lib/qt6/moc $(location tree_scene.hpp) -o $@",
    tags = ["local"],
)

cc_library(
    name = "visualizer_lib",
    srcs = [
        "main_window.cpp",
        "moc_main_window.cpp",
        "moc_tree_scene.cpp",
        "tree_scene.cpp",
    ],
    hdrs = [
        "main_window.hpp",
        "tree_scene.hpp",
    ],
    copts = [
        "-fPIC",
        "-I/usr/include/qt6",
        "-I/usr/include/qt6/QtWidgets",
        "-I/usr/include/qt6/QtGui",
        "-I/usr/include/qt6/QtCore",
    ],
    linkopts = [
        "-L/usr/lib",
        "-lQt6Widgets",
        "-lQt6Gui",
        "-lQt6Core",
        "-lGL",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//data_container",
        "//dataset",
        "//decision_tree:decision_tree_lib",
    ],
)

cc_binary(
    name = "visualizer",
    srcs = ["main.cpp"],
    copts = [
        "-fPIC",
        "-I/usr/include/qt6",
        "-I/usr/include/qt6/QtWidgets",
        "-I/usr/include/qt6/QtGui",
        "-I/usr/include/qt6/QtCore",
    ],
    data = ["//data:iris.data"],
    deps = [":visualizer_lib"],
)
