// Copyright 2022 Google LLC
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

#ifndef LIBHOTH_PROTOCOL_AUTHZ_COMMAND_H_
#define LIBHOTH_PROTOCOL_AUTHZ_COMMAND_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HOTH_PRV_CMD_HOTH_AUTHZ_COMMAND 0x0034

#define AUTHORIZED_COMMAND_SIGNATURE_SIZE 64
#define AUTHORIZED_COMMAND_NONCE_SIZE 32
#define AUTHORIZED_COMMAND_VERSION 1

#define HOTH_PRV_CMD_HOTH_GET_AUTHZ_COMMAND_NONCE 0x0035

struct hoth_authorized_command_get_nonce_response {
  uint32_t nonce[AUTHORIZED_COMMAND_NONCE_SIZE / sizeof(uint32_t)];
  uint32_t supported_key_info;
} __attribute__((packed, aligned(4)));

struct hoth_authorized_command_request {
  uint8_t signature[AUTHORIZED_COMMAND_SIGNATURE_SIZE];
  uint32_t version;
  uint32_t size;
  uint32_t key_info;
  uint32_t dev_id_0;
  uint32_t dev_id_1;
  uint32_t nonce[AUTHORIZED_COMMAND_NONCE_SIZE / sizeof(uint32_t)];
  uint32_t opcode;
  uint32_t arg_bytes[];
} __attribute__((packed, aligned(4)));

#ifdef __cplusplus
}
#endif

#endif  // LIBHOTH_PROTOCOL_AUTHZ_COMMAND_H_
