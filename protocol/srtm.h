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

#ifndef LIBHOTH_PROTOCOL_SRTM_H_
#define LIBHOTH_PROTOCOL_SRTM_H_

#include <stdint.h>

#include "transports/libhoth_device.h"

#ifdef __cplusplus
extern "C" {
#endif

// This command allows callers to push initial measurements into PCR0. This
// command will fail if the TPM has already been started up, or if the
// data to measure exceeds SRTM_DATA_MAX_SIZE_BYTES.
#define HOTH_PRV_CMD_HOTH_SRTM 0x0044
#define SRTM_DATA_MAX_SIZE_BYTES 64

struct hoth_srtm_request {
  uint16_t data_size;
  uint8_t data[SRTM_DATA_MAX_SIZE_BYTES];
} __attribute__((packed, aligned(4)));

// Populates `request` from `measurement`, a hex string of at most
// SRTM_DATA_MAX_SIZE_BYTES bytes. Prints the reason to stderr and returns -1
// if the string is invalid.
int libhoth_srtm_request_from_hex(struct hoth_srtm_request* request,
                                  const char* measurement);

// Pushes the measurement in `request` into PCR0. Returns 0 on success, or the
// libhoth_hostcmd_exec() error code.
int libhoth_srtm(struct libhoth_device* dev,
                 const struct hoth_srtm_request* request);

#ifdef __cplusplus
}
#endif

#endif  // LIBHOTH_PROTOCOL_SRTM_H_
