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

#ifndef LIBHOTH_PROTOCOL_TARGET_RESET_H_
#define LIBHOTH_PROTOCOL_TARGET_RESET_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Options and request struct for HOTH_PRV_CMD_HOTH_RESET_TARGET */
enum hoth_target_reset_option {
  HOTH_TARGET_RESET_OPTION_RELEASE = 0,  // Release target from reset
  HOTH_TARGET_RESET_OPTION_SET = 1,      // Put target in reset
  HOTH_TARGET_RESET_OPTION_PULSE = 2,    // Put target in reset then release
};

#define RESET_TARGET_ID_RSTCTRL0 0
struct hoth_request_reset_target {
  uint32_t target_id;
  // "reset_option" must be one of hoth_target_reset_option
  // Side-by-Side (Hammurabi and Chopper) only support
  // RESET_PULSE. For backward compatibility, if
  // HOTH_PRV_CMD_HOTH_RESET_TARGET is sent without a
  // request param, it defaults to RESET_PULSE. Viperlite
  // and Diorite only support RESET_SET and RESET_RELEASE
  uint8_t reset_option;
  uint8_t reserved[12];
} __attribute__((packed));

/* Reset the target device. */
#define HOTH_PRV_CMD_HOTH_RESET_TARGET 0x0012

// Arm the coordinated reset trigger, which will cause the hoth to perform a
// hard reset when it receives the hardware trigger event.
#define HOTH_PRV_CMD_HOTH_ARM_COORDINATED_RESET 0x001A

#ifdef __cplusplus
}
#endif

#endif  // LIBHOTH_PROTOCOL_TARGET_RESET_H_
