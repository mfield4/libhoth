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

#ifndef LIBHOTH_PROTOCOL_TARGET_CONTROL_H_
#define LIBHOTH_PROTOCOL_TARGET_CONTROL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Control miscellaneous boolean functions on target */
#define HOTH_PRV_CMD_HOTH_TARGET_CONTROL 0x0047

/* Options and request struct for HOTH_PRV_CMD_HOTH_TARGET_CONTROL */
enum hoth_target_control_action {
  // Returns the current enabled/disabled status of the given function.
  HOTH_TARGET_CONTROL_ACTION_GET_STATUS = 0,

  // Changes the status of the given function to "Disabled". Returns the
  // previous enabled/disabled status of the given function.
  HOTH_TARGET_CONTROL_ACTION_DISABLE = 1,

  // Changes the status of the given function to "Enabled". Returns the previous
  // enabled/disabled status of the given function.
  HOTH_TARGET_CONTROL_ACTION_ENABLE = 2,

  // Recommended to be used for `HOTH_TARGET_CONTROL_SBS_MUX_SINGLE` function
  HOTH_TARGET_CONTROL_ACTION_SBS_MUX_SINGLE_CONNECT_FLASH_TO_ROT =
      HOTH_TARGET_CONTROL_ACTION_DISABLE,
  HOTH_TARGET_CONTROL_ACTION_SBS_MUX_SINGLE_CONNECT_FLASH_TO_TARGET =
      HOTH_TARGET_CONTROL_ACTION_ENABLE,

  // Recommended to be used for `HOTH_TARGET_CONTROL_SBS_MUX_DUAL` function
  HOTH_TARGET_CONTROL_ACTION_SBS_MUX_DUAL_CONNECT_TARGET_TO_SPI_FLASH_0 =
      HOTH_TARGET_CONTROL_ACTION_DISABLE,
  HOTH_TARGET_CONTROL_ACTION_SBS_MUX_DUAL_CONNECT_TARGET_TO_SPI_FLASH_1 =
      HOTH_TARGET_CONTROL_ACTION_ENABLE,
};

enum hoth_target_control_function {
  // Allow control over GPIO for Jtag debug enable (if present)
  HOTH_TARGET_CONTROL_DEBUG = 0,
  HOTH_TARGET_CONTROL_RESERVED1 = 1,
  // Allow control over GPIO for I2C Mux select (if present)
  HOTH_TARGET_CONTROL_I2C_MUX = 2,
  // Allow control over GPIO for Generic Mux select (if present)
  HOTH_TARGET_CONTROL_GENERIC_MUX = 3,
  // Allow checking whether external USB host is connected to system in which
  // RoT is present
  HOTH_TARGET_DETECT_EXTERNAL_USB_HOST_PRESENCE = 4,
  HOTH_TARGET_CONTROL_SBS_MUX_SINGLE = 5,
  HOTH_TARGET_CONTROL_SBS_MUX_DUAL = 6,
  HOTH_TARGET_CONTROL_FUNCTION_MAX,
};

enum hoth_target_control_status {
  HOTH_TARGET_CONTROL_STATUS_UNKNOWN = 0,
  HOTH_TARGET_CONTROL_STATUS_DISABLED = 1,
  HOTH_TARGET_CONTROL_STATUS_ENABLED = 2,

  // Recommended to be used for `HOTH_TARGET_DETECT_EXTERNAL_USB_HOST_PRESENCE`
  HOTH_TARGET_EXTERNAL_USB_HOST_NOT_PRESENT =
      HOTH_TARGET_CONTROL_STATUS_DISABLED,
  HOTH_TARGET_EXTERNAL_USB_HOST_PRESENT = HOTH_TARGET_CONTROL_STATUS_ENABLED,

  // Recommended to be used for `HOTH_TARGET_CONTROL_SBS_MUX_SINGLE` function
  HOTH_TARGET_CONTROL_SBS_SINGLE_MUX_CONNECTED_FLASH_TO_ROT =
      HOTH_TARGET_CONTROL_STATUS_DISABLED,
  HOTH_TARGET_CONTROL_SBS_SINGLE_MUX_CONNECTED_FLASH_TO_TARGET =
      HOTH_TARGET_CONTROL_STATUS_ENABLED,

  // Recommended to be used for `HOTH_TARGET_CONTROL_SBS_MUX_DUAL` function
  HOTH_TARGET_CONTROL_SBS_MUX_DUAL_TARGET_CONNECTED_TO_SPI_FLASH_0 =
      HOTH_TARGET_CONTROL_STATUS_DISABLED,
  HOTH_TARGET_CONTROL_SBS_MUX_DUAL_TARGET_CONNECTED_TO_SPI_FLASH_1 =
      HOTH_TARGET_CONTROL_STATUS_ENABLED,
};

struct hoth_request_target_control {
  uint16_t function;  // must be hoth_target_control_function
  uint16_t action;    // must be hoth_target_control_action
  uint8_t args[];     // function+action specific args. Unused right now
} __attribute__((packed, aligned(4)));

struct hoth_response_target_control {
  // If the action changes the target control status, returns the status prior
  // to the change requested by the host command.
  //
  // Must be hoth_target_control_status
  uint16_t status;
} __attribute__((packed, aligned(4)));

#ifdef __cplusplus
}
#endif

#endif  // LIBHOTH_PROTOCOL_TARGET_CONTROL_H_
