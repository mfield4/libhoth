// Copyright 2025 Google LLC
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

#ifndef LIBHOTH_PROTOCOL_STATISTICS_H_
#define LIBHOTH_PROTOCOL_STATISTICS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "protocol/status.h"
#include "transports/libhoth_device.h"

/* Get various statistics */
#define HOTH_PRV_CMD_HOTH_GET_STATISTICS 0x000F

struct boot_timing_data {
  uint32_t start_us;
  uint32_t end_us;
};

struct hoth_response_statistics {
  /*
   * Number of 32 bit words returned from this command.
   * That's not including the reserved fields.
   *  Offset: 0
   */
  uint32_t valid_words;

  /*
   * The set of flags which describe the most recent reset.  See
   * include/system.h RESET_FLAG_* for details.
   *  Offset: 1 (32 bit words)
   */
  uint32_t hoth_reset_flags;

  /*
   * Number of microseconds since the last hoth boot.
   *  Offset: 2
   */
  uint64_t time_since_hoth_boot_us;

  /*
   * The current temperature of the hoth chip. This is just the value
   * in the SUM8 register, no conversion to celsius or fahrenheit is applied.
   * The value returned is a 9.3 bit fixed point binary number. Anything
   * greater than the max value of a 9.3 bit fixed point binary number is
   * considered invalid. Default invalid return value is 0xFFFFFFFF.
   *  Offset: 4
   */
  uint32_t hoth_temperature;

  /*
   * The current INFO strike count in the RO region.
   * Offset: 5
   */
  uint32_t ro_info_strikes;

  /*
   * The current INFO strike count in the RW region.
   * Offset: 6
   */
  uint32_t rw_info_strikes;

  /*
   * For testing, a scratch value to say something
   * Debug only, should be zero in release builds
   * Offset: 7
   */
  uint32_t scratch_value;

  /*
   * Reason code for last payload update failure.
   */
  uint16_t payload_update_failure_reason;

  /*
   * Reason for last firmware update failure.
   */
  uint16_t firmware_update_failure_reason;

  /*
   * Minor version of the last firmware update that failed.
   */
  uint32_t failed_firmware_minor_version;

  /*
   * Time in microseconds of various things we want to measure during
   * bootup.  All times are in microseconds.
   * total - Time from reset to boot up
   * update - Time spent in the self update routine.  Since a proper self update
   *          involves a reset, this time is always expected to be low.
   * mirroring - Time spent mirroing the self-update.  This time is a reasonable
   *             proxy for the total self update time.
   * payload_validation - Time spent validating the payload, copying mutable
   *                      regions and/or dealing with failsafe fallback.
   */
  struct boot_timing_data boot_timing_total;
  struct boot_timing_data boot_timing_firmware_update;
  struct boot_timing_data boot_timing_firmware_mirroring;
  struct boot_timing_data boot_timing_payload_validation;

  /*
   * Confirmation cookie for Payload Update
   */
  uint32_t payload_update_confirmation_cookie_failure_reason;
  uint64_t payload_update_confirmation_cookie;

  /*
   * Error code returned by a bootloader update failure.
   */
  uint32_t bootloader_update_error;

  /*
   * Future expansion.
   */
  uint32_t reserved[42];
} __attribute__((packed));

libhoth_error libhoth_get_statistics(struct libhoth_device* dev,
                                     struct hoth_response_statistics* stats);

enum firmware_update_failure_reason {
  FIRMWARE_UPDATE_SUCCESS = 0,  // HOTH_SUCCESS
  FIRMWARE_UPDATE_NO_HEADER_FOUND = 1,
  FIRMWARE_UPDATE_INVALID_HEADER_SIZE = 2,
  FIRMWARE_UPDATE_INVALID_DESCRIPTOR = 3,
  FIRMWARE_UPDATE_DELIVERY_MECHANISM_MISMATCH = 4,
  FIRMWARE_UPDATE_INVALID_REGION = 5,
  FIRMWARE_UPDATE_VERIFY_BAD_HEADER = 6,
  FIRMWARE_UPDATE_VERIFY_HASH_IMAGE_FAILED = 7,
  FIRMWARE_UPDATE_VERIFY_HASH_FUSE_MAP_FAILED = 8,
  FIRMWARE_UPDATE_VERIFY_HASH_INFO_MAP_FAILED = 9,
  FIRMWARE_UPDATE_VERIFY_SIGNATURE_FAILED = 10,
  FIRMWARE_UPDATE_HASH_IMAGE_FIPS_FAILED = 11,
  FIRMWARE_UPDATE_VERIFY_FIPS_FAILED = 12,
  FIRMWARE_UPDATE_EXTERNAL_AB_HEADER_MISMATCH = 13,
  FIRMWARE_UPDATE_VERSIONS_EQUAL = 14,
  FIRMWARE_UPDATE_FIRST_VERSION_NEWER = 15,
  FIRMWARE_UPDATE_MAUV_UPDATE_NOT_ALLOWED = 16,
  FIRMWARE_UPDATE_EVEN_ODD_ROLLBACK_NOT_ALLOWED = 17,
  FIRMWARE_UPDATE_EVEN_ODD_ROLLBACK_PAYLOAD_TOO_OLD = 18,
  FIRMWARE_UPDATE_MIRROR_VERIFY_FAILED = 19,
  FIRMWARE_UPDATE_MIRROR_RW_FAILED = 20,
  FIRMWARE_UPDATE_MIRROR_RO_FAILED = 21,
  FIRMWARE_UPDATE_VERSION_MATCHES_DENYLIST = 22,
  FIRMWARE_UPDATE_INVALID_DESCRIPTOR_VERSION = 23,
  FIRMWARE_UPDATE_INVALID_RW_KEY_TRANSITION = 24,
  FIRMWARE_UPDATE_ERROR_MAX = 25,
};

// List of common failure codes for payload update.
enum payload_update_failure_reason {
  PAYLOAD_UPDATE_SUCCESS = 0,
  PAYLOAD_UPDATE_VALIDATE_RUNTIME_FAILURE = 1,
  PAYLOAD_UPDATE_VALIDATE_UNSUPPORTED_DESCRIPTOR = 2,
  PAYLOAD_UPDATE_VALIDATE_INVALID_DESCRIPTOR = 3,
  PAYLOAD_UPDATE_VALIDATE_INVALID_IMAGE_FAMILY = 4,
  PAYLOAD_UPDATE_VALIDATE_IMAGE_TYPE_DISALLOWED = 5,
  PAYLOAD_UPDATE_VALIDATE_DENYLISTED_VERSION = 6,
  PAYLOAD_UPDATE_VALIDATE_UNTRUSTED_KEY = 7,
  PAYLOAD_UPDATE_VALIDATE_INVALID_SIGNATURE = 8,
  PAYLOAD_UPDATE_VALIDATE_INVALID_HASH = 9,
  PAYLOAD_UPDATE_VALIDATE_PENDING = 10,
  PAYLOAD_UPDATE_VALIDATE_INVALID_SESSION_ID = 11,
  PAYLOAD_UPDATE_VALIDATE_FINGERPRINT_NOT_FOUND = 12,
  PAYLOAD_UPDATE_VALIDATE_UNSUPPORTED_FINGERPRINT_HASH_TYPE = 13,
  PAYLOAD_UPDATE_VALIDATE_MISSING_BOOT_HASH = 14,
  PAYLOAD_UPDATE_VALIDATE_UNEXPECTED_SKIP_BOOT_VALIDATION_REGION = 15,
  PAYLOAD_UPDATE_VALIDATE_MULTIPLE_DESCRIPTORS_FOUND = 16,
  PAYLOAD_UPDATE_VALIDATE_RESERVED_8 = 17,
  PAYLOAD_UPDATE_VALIDATE_RESERVED_9 = 18,
  PAYLOAD_UPDATE_VALIDATE_RESERVED_10 = 19,
  PAYLOAD_UPDATE_VALIDATE_RESERVED_11 = 20,
  PAYLOAD_UPDATE_ERASE_FAILED = 21,
  PAYLOAD_UPDATE_WRITE_FAILED = 22,
  PAYLOAD_UPDATE_READ_FAILED = 23,
  PAYLOAD_UPDATE_INVALID_PARAMS = 24,
  PAYLOAD_UPDATE_INVALID_STAGING_OFFSET = 25,
  PAYLOAD_UPDATE_INVALID_STAGING_SIZE = 26,
  PAYLOAD_UPDATE_FILTER_CALLBACK_FAILED = 27,
  PAYLOAD_UPDATE_ABORT_PENDING_UPDATE_FAILED = 28,
  PAYLOAD_UPDATE_BAD_PACKET_HEADER = 29,
  PAYLOAD_UPDATE_FPGA_UPDATE_HEADER_FAILED = 30,
  PAYLOAD_UPDATE_REGIONS_NOT_COMPATIBLE_FOR_MIGRATION = 31,
  PAYLOAD_UPDATE_MAUV_DOES_NOT_ALLOW_UPDATE = 32,
  PAYLOAD_UPDATE_STAGING_AREA_INVALID = 33,
  PAYLOAD_UPDATE_SET_ACTIVE_HALF_FAILED = 34,
  PAYLOAD_UPDATE_CALLBACK_FAILED = 35,
  PAYLOAD_UPDATE_SET_PENDING_MIGRATION_FAILED = 36,
  PAYLOAD_UPDATE_CONFIRM_INVALID_TIMEOUT = 37,
  PAYLOAD_UPDATE_CONFIRM_NOT_ENABLED = 38,
  PAYLOAD_UPDATE_CONFIRM_NO_UPDATE_PAYLOAD = 39,
  PAYLOAD_UPDATE_CONFIRM_NO_PENDING_PAYLOAD = 40,
  PAYLOAD_UPDATE_CONFIRM_GNVRAM_ERROR = 41,
  PAYLOAD_UPDATE_CONFIRM_REVERT_PAYLOAD = 42,
  PAYLOAD_UPDATE_CONFIRM_NOT_SUPPORTED = 43,
  PAYLOAD_UPDATE_GNVRAM_READ_ERROR = 44,
  PAYLOAD_UPDATE_GNVRAM_WRITE_ERROR = 45,
  PAYLOAD_UPDATE_ERROR_MAX = 46,
};

/* Hoth reset causes, as reported in hoth_response_statistics.hoth_reset_flags
 */
enum hoth_reset_flag {
  kResetFlagOther = 1u << 0,       /* Other known reason */
  kResetFlagResetPin = 1u << 1,    /* Reset pin asserted */
  kResetFlagBrownout = 1u << 2,    /* Brownout */
  kResetFlagPowerOn = 1u << 3,     /* Power-On reset */
  kResetFlagWatchdog = 1u << 4,    /* Watchdog timer reset */
  kResetFlagSoft = 1u << 5,        /* Soft reset trigger by core */
  kResetFlagHibernate = 1u << 6,   /* Wake from hibernate */
  kResetFlagRtcAlarm = 1u << 7,    /* RTC alarm wake */
  kResetFlagWakePin = 1u << 8,     /* Wake pin triggered wake */
  kResetFlagLowBattery = 1u << 9,  /* Low battery triggered wake */
  kResetFlagSysjump = 1u << 10,    /* Jumped directly to this image */
  kResetFlagHard = 1u << 11,       /* Hard reset from software */
  kResetFlagApOff = 1u << 12,      /* Do not power on AP */
  kResetFlagPreserved = 1u << 13,  /* Some flags preserved from last boot */
  kResetFlagUsbResume = 1u << 14,  /* USB Resume triggered wake */
  kResetFlagRdd = 1u << 15,        /* USB Type-C debug cable */
  kResetFlagRbox = 1u << 16,       /* Fixed reset functionality */
  kResetFlagSecurity = 1u << 17,   /* Security threat */
  kResetFlagApWatchdog = 1u << 18, /* AP watchdog reset */
};

// Returns the name of a firmware_update_failure_reason, or "Invalid Status
// Code" for unknown values.
const char* libhoth_firmware_update_failure_reason_string(uint16_t reason);

// Returns the name of a payload_update_failure_reason, or "Invalid Status
// Code" for unknown values.
const char* libhoth_payload_update_failure_reason_string(uint16_t reason);

#ifdef __cplusplus
}
#endif

#endif
