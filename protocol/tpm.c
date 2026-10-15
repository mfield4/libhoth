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

#include "protocol/tpm.h"

#include <stddef.h>

#include "protocol/host_cmd.h"

int libhoth_tpm_set_mode(struct libhoth_device* dev, enum ec_tpm_mode mode) {
  struct tpm_mode req = {
      .mode = mode,
  };
  return libhoth_hostcmd_exec(
      dev, HOTH_CMD_BOARD_SPECIFIC_BASE + HOTH_PRV_CMD_SET_TPM_MODE, 0, &req,
      sizeof(req), NULL, 0, NULL);
}

int libhoth_tpm_get_mode(struct libhoth_device* dev, struct tpm_mode* mode) {
  return libhoth_hostcmd_exec(
      dev, HOTH_CMD_BOARD_SPECIFIC_BASE + HOTH_PRV_CMD_GET_TPM_MODE, 0, NULL, 0,
      mode, sizeof(*mode), NULL);
}
