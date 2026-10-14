load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

# Compat: pie-rot tests/libhoth/dfu_update.rs hardcodes the runfiles path
# "libhoth/examples/htool". An alias would NOT preserve that path. Remove after
# downstream migrates to //tools/htool:htool.
cc_binary(
    name = "htool",
    srcs = ["//tools/htool:main.c"],
    features = ["fully_static_link_target"],
    deps = [
        "//:git_version",
        "//tools/htool:htool_lib",
    ],
)

# Template for a custom htool with extra commands and transports.
cc_binary(
    name = "htool_extension_example",
    srcs = ["htool_extension_example.c"],
    deps = [
        "//tools/common:htool_cmd",
        "//tools/htool:htool_lib",
    ],
)
