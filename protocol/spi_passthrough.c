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

#include "protocol/spi_passthrough.h"

#include <stdbool.h>
#include <stddef.h>

#include "protocol/host_cmd.h"

int libhoth_spi_passthrough_set(struct libhoth_device* dev, bool enable) {
  return libhoth_hostcmd_exec(
      dev,
      HOTH_CMD_BOARD_SPECIFIC_BASE +
          (enable ? HOTH_PRV_CMD_HOTH_SPS_PASSTHROUGH_ENABLE
                  : HOTH_PRV_CMD_HOTH_SPS_PASSTHROUGH_DISABLE),
      /*version=*/0, NULL, 0, NULL, 0, NULL);
}

int libhoth_flash_spi_info(struct libhoth_device* dev,
                           struct hoth_response_flash_spi_info* info) {
  return libhoth_hostcmd_exec(dev, HOTH_CMD_FLASH_SPI_INFO, /*version=*/0, NULL,
                              0, info, sizeof(*info), NULL);
}
