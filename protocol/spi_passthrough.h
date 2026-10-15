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

#ifndef LIBHOTH_PROTOCOL_SPI_PASSTHROUGH_H_
#define LIBHOTH_PROTOCOL_SPI_PASSTHROUGH_H_

#include <stdbool.h>
#include <stdint.h>

#include "protocol/host_cmd.h"
#include "transports/libhoth_device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HOTH_CMD_FLASH_SPI_INFO 0x0018

struct hoth_response_flash_spi_info {
  /* JEDEC info from command 0x9F (manufacturer, memory type, size) */
  uint8_t jedec[3];

  /* Pad byte; currently always contains 0 */
  uint8_t reserved0;

  /* Manufacturer / device ID from command 0x90 */
  uint8_t mfr_dev_id[2];

  /* Status registers from command 0x05 and 0x35 */
  uint8_t sr1, sr2;
} __hoth_align1;

// After sending this command, any future synchronous SPI reads from the RoT's
// SPI-slave interface will return all zeroes, but out-of-band methods (such as
// HOTH_SPI_OPERATION via USB) will be able to interact with the SPI flash.
#define HOTH_PRV_CMD_HOTH_SPS_PASSTHROUGH_DISABLE 0x003b

// Re-enables SPS passthrough. Future out-of-band access to the SPI flash will
// fail.
#define HOTH_PRV_CMD_HOTH_SPS_PASSTHROUGH_ENABLE 0x003c

// Enables or disables SPS->SPI passthrough. Returns 0 on success, or the
// libhoth_hostcmd_exec() error code.
int libhoth_spi_passthrough_set(struct libhoth_device* dev, bool enable);

// Reads the SPI flash JEDEC ID, device ID and status registers. Returns 0 on
// success, or the libhoth_hostcmd_exec() error code.
int libhoth_flash_spi_info(struct libhoth_device* dev,
                           struct hoth_response_flash_spi_info* info);

#ifdef __cplusplus
}
#endif

#endif  // LIBHOTH_PROTOCOL_SPI_PASSTHROUGH_H_
