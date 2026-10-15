// Copyright 2026 Google LLC
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

#include "protocol/security_info.h"

#include <stddef.h>
#include <stdint.h>

#include "protocol/security_v2.h"

int libhoth_security_info_get(struct libhoth_device* dev,
                              struct hoth_info* info) {
  // Send Info Request
  uint8_t request_storage_hdr[HOTH_SECURITY_V2_REQUEST_SIZE(0)] = {};
  uint8_t response_storage_hdr[HOTH_SECURITY_V2_RESPONSE_SIZE(1) +
                               sizeof(struct hoth_info)] = {};

  struct libhoth_security_v2_param response_params[] = {
      {
          .data = info,
          .size = sizeof(struct hoth_info),
      },
  };
  return libhoth_security_v2_exec(
      dev, /*major=*/HOTH_PRV_CMD_HOTH_SECURITY_V2_INFO_MAJOR_COMMAND,
      /*minor=*/0,
      /*base_command=*/HOTH_BASE_CMD(HOTH_PRV_CMD_HOTH_SECURITY_V2),
      LIBHOTH_SECURITY_V2_BUFFER_PARAM(request_storage_hdr), NULL, 0,
      LIBHOTH_SECURITY_V2_BUFFER_PARAM(response_storage_hdr), response_params,
      sizeof(response_params) / sizeof(response_params[0]));
}
