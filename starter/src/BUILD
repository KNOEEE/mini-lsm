# modified from
# https://github.com/baidu/BaikalDB/blob/master/third-party/leveldb.BUILD
load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])


SOURCES = [
  "db/db_impl.cpp",
  "db/db_iter.cpp",
  "db/dbformat.cpp",
  "db/memtable.cpp",
  "db/version_set.cpp",
  "db/write_batch.cpp",
  "table/block.cpp",
  "table/block_builder.cpp",
  "table/format.cpp",
  "table/iterator.cpp",
  "table/merger.cpp",
  "util/arena.cpp",
  "util/coding.cpp",
  "util/crc32c.cpp",
  "util/env.cpp",
  "util/env_posix.cpp",
  "util/logging.cpp",
  "util/comparator.cpp",
  "util/options.cpp",
  "util/status.cpp",
]


cc_library(
    name = "minilsm",
    srcs = SOURCES,
    hdrs = glob([
        "util/*.h",
        "table/*.h",
        "db/*.h",
        "include/minilsm/*.h",
        "port/*.h"
    ],
    exclude = [
      "**/*test.*",
    ]),
    includes = [
        "include/",
    ],
    deps = [
      "@snappy//:snappy",
    ],
    defines = ["HAVE_SNAPPY"],
    copts = [
      "-g",
      # "-fno-stack-protector",
      "-std=c++17",
    ]
)

cc_binary(
  name = "demo",
  srcs = [
    "example/demo.cpp"
  ],
  deps = [
    ":minilsm"
  ],
  copts = [
      # Disable stack protection to deal with
      # "ld.lld: error: undefined symbol: __stack_chk_guard"
      "-fno-stack-protector",
  ],
  linkstatic = True,
)

cc_binary(
  name = "open",
  srcs = [
    "example/open.cpp"
  ],
  deps = [
    ":minilsm"
  ],
  copts = [
      # Disable stack protection to deal with
      # "ld.lld: error: undefined symbol: __stack_chk_guard"
      "-fno-stack-protector",
  ],
  linkstatic = True,
)

cc_test(
    name = "hello_test",
    size = "small",
    srcs = [
      "db/skiplist_test.cpp",
      "port/concurrent_test.cpp",
      "port/snappy_test.cpp",
      "util/cast_test.cpp",
      "util/env_posix_test.cpp",
      "table/format_test.cpp",
      "tests/study/smart_ptr_test.cpp",
      "tests/study/sync_test.cpp",
    ],
    deps = [
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        ":minilsm",
    ],
)

cc_binary(
  name = "bench",
  srcs = [
    "tests/microbench/basic.cpp",
    "tests/microbench/args.cpp",
  ],
  deps = [
    '@google_benchmark//:benchmark',
    '@google_benchmark//:benchmark_main',
  ],
  copts = [
      # Disable stack protection to deal with
      # "ld.lld: error: undefined symbol: __stack_chk_guard"
      # "-fstack-protector",
  ],
  linkstatic = True,
  # to deal with
  # "ld.lld: error: undefined symbol: __stack_chk_guard"
  linkopts=["-fstack-protector"],
)

cc_binary(
  name = "coro",
  # Folly requires GCC 10+; keep this C++20 study target opt-in on older systems.
  tags = ["manual"],
  srcs = [
    "tests/study/coro.cpp"
  ],
  deps = [
    "@folly//folly/executors:global_executor",
    "@folly//folly/experimental/coro:blocking_wait",
    "@folly//folly/experimental/coro:task",
    "@folly//folly/futures:futures",
    "@folly//folly/init:init",
  ],
  copts = [
    "-std=c++20",
  ],
  # linkstatic = True
)
# cc_binary(
#   name = "coro",
#   srcs = [
#     "tests/study/coro.cpp"
#   ],
#   deps = [
#     "@folly//folly/init",
#     # "@folly//folly/executors",
#     # "@folly//folly/experimental/coro:task",
#     # "@folly//folly/experimental/coro:blocking_wait",
#     # "@folly//folly/experimental/coro:generator",
#   ],
#   copts = [
#     "-std=c++2a",
#   ],
#   linkstatic = True
# )
