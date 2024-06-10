# modified from 
# https://github.com/baidu/BaikalDB/blob/master/third-party/leveldb.BUILD
package(default_visibility = ["//visibility:public"])


SOURCES = [
  "db/dbformat.cpp",
  "util/arena.cpp",
  "util/coding.cpp",
  "util/logging.cpp"
    ]

cc_library(
    name = "minilsm",
    srcs = SOURCES,
    hdrs = glob([ 
        "util/*.h",
        "table/*.h",
        "db/*.h",
        "include/minilsm/*.h"
    ],
    exclude = [
            "**/*test.*",
    ]),
    includes = [
        "include/",
    ],
)

cc_binary(
  name = "demo",
  srcs = [
    "example/demo.cpp"
  ],
  deps = [
    ":minilsm"
  ],
  linkstatic = True,
)