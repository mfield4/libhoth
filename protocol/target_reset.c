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

#include "protocol/target_reset.h"

#include <stddef.h>

#include "protocol/host_cmd.h"

int libhoth_target_reset(struct libhoth_device* dev,
                         enum hoth_target_reset_option option) {
  struct hoth_request_reset_target req = {
      .target_id = RESET_TARGET_ID_RSTCTRL0,
      .reset_option = option,
  };
  return libhoth_hostcmd_exec(
      dev, HOTH_CMD_BOARD_SPECIFIC_BASE + HOTH_PRV_CMD_HOTH_RESET_TARGET, 0,
      &req, sizeof(req), NULL, 0, NULL);
}

int libhoth_arm_coordinated_reset(struct libhoth_device* dev) {
  return libhoth_hostcmd_exec(
      dev,
      HOTH_CMD_BOARD_SPECIFIC_BASE + HOTH_PRV_CMD_HOTH_ARM_COORDINATED_RESET,
      /*version=*/0, NULL, 0, NULL, 0, NULL);
}
