// Copyright 2023 Google LLC
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

#include "htool_statistics.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "host_commands.h"
#include "htool.h"
#include "protocol/statistics.h"

#define STATISTIC_OFFSET(field) \
  (offsetof(struct hoth_response_statistics, field) / sizeof(uint32_t))

int htool_statistics(const struct htool_invocation* inv) {
  (void)inv;
  struct libhoth_device* dev = htool_libhoth_device();
  if (!dev) {
    return -1;
  }

  struct hoth_response_statistics stat;
  libhoth_error err = libhoth_get_statistics(dev, &stat);
  if (err != HOTH_SUCCESS) {
    htool_report_error("get_statistics", err);
    return -1;
  }

  printf("Valid statistics words: %u\n", stat.valid_words);
  uint32_t flags = stat.hoth_reset_flags;
  printf("Reset flags: 0x%08x\n", flags);
  const char kIndent[] = "             %s\n";
  // clang-format off
  if (flags & kResetFlagOther)      printf(kIndent, "ResetFlagOther");
  if (flags & kResetFlagResetPin)   printf(kIndent, "ResetFlagResetPin");
  if (flags & kResetFlagBrownout)   printf(kIndent, "ResetFlagBrownout");
  if (flags & kResetFlagPowerOn)    printf(kIndent, "ResetFlagPowerOn");
  if (flags & kResetFlagWatchdog)   printf(kIndent, "ResetFlagWatchdog");
  if (flags & kResetFlagSoft)       printf(kIndent, "ResetFlagSoft");
  if (flags & kResetFlagHibernate)  printf(kIndent, "ResetFlagHibernate");
  if (flags & kResetFlagRtcAlarm)   printf(kIndent, "ResetFlagRtcAlarm");
  if (flags & kResetFlagWakePin)    printf(kIndent, "ResetFlagWakePin");
  if (flags & kResetFlagLowBattery) printf(kIndent, "ResetFlagLowBattery");
  if (flags & kResetFlagSysjump)    printf(kIndent, "ResetFlagSysjump");
  if (flags & kResetFlagHard)       printf(kIndent, "ResetFlagHard");
  if (flags & kResetFlagApOff)      printf(kIndent, "ResetFlagApOff");
  if (flags & kResetFlagPreserved)  printf(kIndent, "ResetFlagPreserved");
  if (flags & kResetFlagUsbResume)  printf(kIndent, "ResetFlagUsbResume");
  if (flags & kResetFlagRdd)        printf(kIndent, "ResetFlagRdd");
  if (flags & kResetFlagRbox)       printf(kIndent, "ResetFlagRbox");
  if (flags & kResetFlagSecurity)   printf(kIndent, "ResetFlagSecurity");
  if (flags & kResetFlagApWatchdog) printf(kIndent, "ResetFlagApWatchdog");
  // clang-format on

  printf("Time since boot: %" PRIu64 " us\n", stat.time_since_hoth_boot_us);
  if (stat.hoth_temperature == 0xFFFFFFFF) {
    printf("Temperature: (invalid)\n");
  } else {
    // The temperature is expressed as a 9.3b fixed point number.
    // Divide the fixed-point value by 8 to get a floating point value.
    // Unfortunately, the temperature sensor is an uncalibrated ADC and the
    // raw value needs to be converted into proper units.  Sadly, no-one
    // seems to be be doing that, so we just report the raw value.
    double temperature = stat.hoth_temperature / 8.0;
    printf("Temperature: %.3f (raw value)\n", temperature);
  }

  if (stat.valid_words > STATISTIC_OFFSET(ro_info_strikes)) {
    printf("ro info strikes: %u\n", stat.ro_info_strikes);
  }

  if (stat.valid_words > STATISTIC_OFFSET(rw_info_strikes)) {
    printf("rw info strikes: %u\n", stat.rw_info_strikes);
  }

  if (stat.valid_words > STATISTIC_OFFSET(scratch_value)) {
    printf("Debug scratch: %u\n", stat.scratch_value);
  }

  if (stat.valid_words > STATISTIC_OFFSET(payload_update_failure_reason)) {
    printf("Payload update failure reason: %s (%u)\n",
           libhoth_payload_update_failure_reason_string(
               stat.payload_update_failure_reason),
           stat.payload_update_failure_reason);
  }

  if (stat.valid_words > STATISTIC_OFFSET(firmware_update_failure_reason)) {
    printf("Firmware update failure reason: %s (%u)\n",
           libhoth_firmware_update_failure_reason_string(
               stat.firmware_update_failure_reason),
           stat.firmware_update_failure_reason);
  }

  if (stat.valid_words > STATISTIC_OFFSET(failed_firmware_minor_version)) {
    printf("Failed firmware minor version: %u\n",
           stat.failed_firmware_minor_version);
  }

  if (stat.valid_words > STATISTIC_OFFSET(boot_timing_payload_validation)) {
    printf("Total boot time:         [%u, %u]us\n",
           stat.boot_timing_total.start_us, stat.boot_timing_total.end_us);
    printf("Payload validation time: [%u, %u]us\n",
           stat.boot_timing_payload_validation.start_us,
           stat.boot_timing_payload_validation.end_us);
    printf("Firmware update time:    [%u, %u]us\n",
           stat.boot_timing_firmware_update.start_us,
           stat.boot_timing_firmware_update.end_us);
    printf("Firmware mirroring time: [%u, %u]us\n",
           stat.boot_timing_firmware_mirroring.start_us,
           stat.boot_timing_firmware_mirroring.end_us);
  }

  if (stat.valid_words > STATISTIC_OFFSET(payload_update_confirmation_cookie)) {
    if (stat.payload_update_confirmation_cookie_failure_reason !=
        PAYLOAD_UPDATE_SUCCESS) {
      printf("Payload Update Confirmation Cookie: failed with %s (%u)\n",
             libhoth_payload_update_failure_reason_string(
                 stat.payload_update_confirmation_cookie_failure_reason),
             stat.payload_update_confirmation_cookie_failure_reason);
    } else {
      printf("Payload Update Confirmation Cookie: %" PRIu64 "\n",
             stat.payload_update_confirmation_cookie);
    }
  }

  if (stat.valid_words > STATISTIC_OFFSET(bootloader_update_error)) {
    if (stat.bootloader_update_error) {
      printf("Bootloader update error: 0x%08x\n", stat.bootloader_update_error);
    } else {
      printf("Bootloader update error: 0x0 (no error)\n");
    }
  }
  return 0;
}
