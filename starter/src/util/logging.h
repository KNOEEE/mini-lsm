// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Must not be included from any .h files to avoid polluting the namespace
// with macros.

#ifndef MINILSM_UTIL_LOGGING_H_
#define MINILSM_UTIL_LOGGING_H_

#include <cstdint>
#include <cstdio>
#include <string>

namespace minilsm {

class Slice;

// Append a human-readable printout of "value" to *str.
// Escapes any non-printable characters found in "value".
void AppendEscapedStringTo(std::string* str, const Slice& value);

// Return a human-readable version of "value".
// Escapes any non-printable characters found in "value".
std::string EscapeString(const Slice& value);


}  // namespace minilsm

#endif  // MINILSM_UTIL_LOGGING_H_
