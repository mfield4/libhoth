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

#include "protocol/authz_command.h"

#include <stdint.h>
#include <string.h>

struct hoth_authorized_command_request libhoth_authz_command_build_request(
    uint64_t hardware_identity, uint32_t opcode, uint32_t key_info,
    const uint32_t* nonce) {
  struct hoth_authorized_command_request request = {
      .version = AUTHORIZED_COMMAND_VERSION,
      .size = sizeof(request),
      .key_info = key_info,
      .dev_id_0 = hardware_identity & UINT32_C(0xffffffff),
      .dev_id_1 = hardware_identity >> 32,
      .opcode = opcode,
  };
  memcpy(request.nonce, nonce, sizeof(request.nonce));
  return request;
}
