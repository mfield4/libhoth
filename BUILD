load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")

package(default_visibility = ["//visibility:public"])

refresh_compile_commands(
    name = "refresh_compile_commands",
    targets = {
        "//" + "...": "",
    },
)

exports_files([
    "third_party/libusb.BUILD",
    "third_party/libusb.patch",
])

cc_library(
    name = "git_version",
    hdrs = [":gen_version_header"],
)

genrule(
    name = "gen_version_header",
    outs = ["git_version.h"],
    cmd = "$(location :print_version_header.sh) > \"$@\"",
    stamp = 1,
    tools = [":print_version_header.sh"],
)

cc_library(
    name = "libhoth_transports_headers",
    hdrs = [
        "//transports:headers",
    ],
    include_prefix = "libhoth/transports",
    strip_include_prefix = "transports",
)

cc_library(
    name = "libhoth_transports_headers_legacy",
    hdrs = [
        "//transports:headers",
    ],
    include_prefix = "transports",
    strip_include_prefix = "transports",
)

cc_library(
    name = "libhoth",
    visibility = ["//visibility:public"],
    deps = [
        ":libhoth_transports_headers",
        ":libhoth_transports_headers_legacy",
        "//transports:libhoth_device",
        "//transports:libhoth_mtd",
        "//transports:libhoth_spi",
        "//transports:libhoth_usb",
    ],
)

alias(
    name = "libusb",
    actual = "@libusb//:libusb",
    visibility = ["//visibility:public"],
)
