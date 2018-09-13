cc_library(
    name = "fenwick",
    hdrs = ["fenwick.h"],
)

cc_library(
    name = "fenwick_2d",
    hdrs = ["fenwick_2d.h"],
)

cc_library(
    name = "fenwick_rurq",
    hdrs = ["fenwick_rurq.h"],
    deps = [
        ":fenwick",
    ],
)

cc_library(
    name = "fenwick_rmq",
    hdrs = ["fenwick_rmq.h"],
)

cc_binary(
    name = "fenwick_benchmark",
    srcs = ["fenwick_benchmark.cc"],
    deps = [
        ":fenwick",
        "@third_party_absl//absl/strings",
        "@third_party_absl//absl/time",
    ],
    linkstatic = True,
)

cc_binary(
    name = "fenwick_2d_benchmark",
    srcs = ["fenwick_2d_benchmark.cc"],
    deps = [
        ":fenwick_2d",
        "@third_party_absl//absl/strings",
        "@third_party_absl//absl/time",
    ],
    linkstatic = True,
)

cc_binary(
    name = "fenwick_rurq_benchmark",
    srcs = ["fenwick_rurq_benchmark.cc"],
    deps = [
        ":fenwick_rurq",
        "@third_party_absl//absl/strings",
        "@third_party_absl//absl/time",
    ],
    linkstatic = True,
)

cc_binary(
    name = "fenwick_rmq_test",
    srcs = ["fenwick_rmq_test.cc"],
    deps = [
        ":fenwick_rmq",
    ],
    linkstatic = True,
)

cc_binary(
    name = "fenwick_rmq_benchmark",
    srcs = ["fenwick_rmq_benchmark.cc"],
    deps = [
        ":fenwick_rmq",
        "@third_party_absl//absl/strings",
        "@third_party_absl//absl/time",
    ],
    linkstatic = True,
)
