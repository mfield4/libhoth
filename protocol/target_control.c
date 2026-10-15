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

#include "protocol/target_control.h"

#include <stddef.h>
#include <stdio.h>

#include "protocol/host_cmd.h"

int libhoth_target_control(struct libhoth_device* dev,
                           enum hoth_target_control_function function,
                           enum hoth_target_control_action action,
                           struct hoth_response_target_control* response) {
  if (response == NULL) {
    return -1;
  }

  const struct hoth_request_target_control request = {
      .function = function,
      .action = action,
  };

  size_t response_length = 0;
  int ret = libhoth_hostcmd_exec(
      dev, HOTH_CMD_BOARD_SPECIFIC_BASE + HOTH_PRV_CMD_HOTH_TARGET_CONTROL,
      /*version=*/0, &request, sizeof(request), response, sizeof(*response),
      &response_length);
  if (ret != 0) {
    fprintf(stderr, "HOTH_TARGET_CONTROL error code: %d\n", ret);
    return ret;
  }
  if (response_length != sizeof(*response)) {
    fprintf(
        stderr,
        "HOTH_TARGET_CONTROL expected exactly %zu response bytes, got %zu\n",
        sizeof(*response), response_length);
    return -1;
  }

  return 0;
}
