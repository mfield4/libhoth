load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

# Template for a custom htool with extra commands and transports.
cc_binary(
    name = "htool_extension_example",
    srcs = ["htool_extension_example.c"],
    deps = [
        "//tools/common:htool_cmd",
        "//tools/htool:htool_lib",
    ],
)
