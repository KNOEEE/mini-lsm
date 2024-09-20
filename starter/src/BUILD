# modified from 
# https://github.com/baidu/BaikalDB/blob/master/third-party/leveldb.BUILD
package(default_visibility = ["//visibility:public"])


SOURCES = [
  "db/db_impl.cpp",
  "db/dbformat.cpp",
  "db/memtable.cpp",
  "db/version_set.cpp",
  "db/write_batch.cpp",
  "table/iterator.cpp",
  "table/merger.cpp",
  "util/arena.cpp",
  "util/coding.cpp",
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
    copts = [
        "-fno-stack-protector",
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