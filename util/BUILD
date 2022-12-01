load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "a_star",
    hdrs = ["a_star.h"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)


cc_library(
    name = "dimension_grid",
    hdrs = ["dimension_grid.h"],
    deps = [
        "@com_google_absl//absl/log:check",
    ],
)

cc_library(
    name = "grid2",
    srcs = ["grid2.cc"],
    hdrs = ["grid2.h"],
    deps = [
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "io",
    srcs = ["io.cc"],
    hdrs = ["io.h"],
    deps = [
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "ratio",
    hdrs = ["ratio.h"],
)
