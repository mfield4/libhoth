// Copyright 2025 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef LIBHOTH_TOOLS_HTOOL_HTOOL_SECURITY_INFO_H_
#define LIBHOTH_TOOLS_HTOOL_HTOOL_SECURITY_INFO_H_

#include <stdint.h>

#include "protocol/security_info.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
struct htool_invocation;

// Retrieve the Info from firmware.
int htool_info(const struct htool_invocation* inv);
#ifdef __cplusplus
}
#endif
#endif  // LIBHOTH_TOOLS_HTOOL_HTOOL_SECURITY_INFO_H_
