# modified from
# https://github.com/baidu/BaikalDB/blob/master/third-party/leveldb.BUILD
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
    # https://bazel.build/reference/be/c-cpp#cc_test.linkopts
    linkopts = [
      # https://stackoverflow.com/questions/6562403/i-dont-understand-wl-rpath-wl
      # https://blog.csdn.net/jiesunliu3215/article/details/112857020?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ECtr-2-112857020-blog-75282883.235%5Ev43%5Epc_blog_bottom_relevance_base7&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ECtr-2-112857020-blog-75282883.235%5Ev43%5Epc_blog_bottom_relevance_base7&utm_relevant_index=3
      # TODO what if libssp is somewhere else dir, how to specify that path
      "-lssp"
    ],
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
      "util/cast_test.cpp",
      "util/env_posix_test.cpp",
      "table/format_test.cpp"
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
    "tests/microbench/basic.cpp"
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