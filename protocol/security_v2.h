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

#ifndef LIBHOTH_PROTOCOL_SECURITY_V2_H_
#define LIBHOTH_PROTOCOL_SECURITY_V2_H_

#include <stddef.h>
#include <stdint.h>

#include "protocol/host_cmd.h"
#include "transports/libhoth_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The identifier for the SecurityV2 host command. All SecurityV2 commands are
 * handled by a single host command handler. Specific SecurityV2 commands are
 * identified by the "major_command" and "minor_command" fields of a command's
 * request header.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2 0x0003

/* Reports on whether a command is allowed to run given the current host command
 * filtering that the firmware is using.
 */
#define HOTH_PRV_CMD_HOTH_IS_HOST_COMMAND_SUPPORTED 0x0011

/**
 * The identifier for the SecurityV3 host command. All SecurityV3 commands are
 * handled by a single host command handler.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V3 0x0033

/**
 * Macro to retrieve the final address of a command, which is the sum of the
 * base address and the added command.
 */
#define HOTH_BASE_CMD(cmd) (HOTH_CMD_BOARD_SPECIFIC_BASE + (cmd))

/**
 * The request header structure for security v2 commands
 */
struct hoth_security_v2_request_header {
  /* The major and minor command codes that identify a specific SecurityV2
   * command. */
  uint8_t major_command;
  uint8_t minor_command;

  /* The number of parameters that follow the header in the request. */
  uint16_t param_count;
} __attribute__((packed));

/**
 * The response header structure for a security v2 command
 */
struct hoth_security_v2_response_header {
  /* The number of parameters that follow the header in the response. */
  uint16_t param_count;

  /* Reserved value, set to 0; used for 32-bit alignment. */
  uint16_t reserved;
} __attribute__((packed));

/**
 * The parameter structure for security v2 commands
 *
 * Parameters follow request/response headers and are 32-bit aligned.
 */
struct hoth_security_v2_parameter {
  /* The number of bytes in the parameter's value. */
  uint16_t size;

  /* Reserved value, set to 0; used for 32-bit alignment. */
  uint16_t reserved;

  /* The bytes representing the value of the parameter should immediately follow
   * this struct. */
} __attribute__((packed));

/**
 * The size of the parameter being passed in a security v2 command
 */
#define HOTH_SECURITY_V2_PARAM_OVERHEAD \
  (sizeof(struct hoth_security_v2_parameter))

/**
 * The size of the request for a security v2 command
 */
#define HOTH_SECURITY_V2_REQUEST_SIZE(param_count)  \
  (sizeof(struct hoth_security_v2_request_header) + \
   (param_count) * HOTH_SECURITY_V2_PARAM_OVERHEAD)

/**
 * The size of the response for a security v2 command
 */
#define HOTH_SECURITY_V2_RESPONSE_SIZE(param_count)  \
  (sizeof(struct hoth_security_v2_response_header) + \
   (param_count) * HOTH_SECURITY_V2_PARAM_OVERHEAD)

/**
 * The command needed to get any certificates from the device.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_GET_CERTIFICATES_MAJOR_COMMAND 1

/**
 * The command to get the alias key certificate from the device.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_GET_ALIAS_KEY_MINOR_COMMAND 20

/**
 * The command to get the device id certificates from the device.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_GET_DEVICE_ID_MINOR_COMMAND 19

/**
 * The command to get the attestation public certificate from the device.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_GET_ATTESTATION_PUB_CERT_MINOR_COMMAND 4

/**
 * The command to get the signed attestation public certificates ferom the
 * device.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_GET_SIGNED_ATTESTATION_PUB_CERT_MINOR_COMMAND \
  25

/**
 * The command to load a signed attestation key certificate and wrapped key onto
 * the device.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_LOAD_ATTESTATION_KEY_MINOR_COMMAND 1

/**
 * The command to unload the attestation key from the device.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_UNLOAD_ATTESTATION_KEY_MINOR_COMMAND 3

/**
 * The command to generate an attestation key using CSR v1 on the device.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_GEN_ATTESTATION_KEY_USING_CSR_V1_MINOR_COMMAND \
  15

/**
 * The command to load a self-signed attestation key from CSR v1 on the device
 * (requires firmware built with self-signed attestation key support).
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_LOAD_ATTESTATION_KEY_FROM_CSR_V1_MINOR_COMMAND \
  16

/**
 * The command to generate a versioned attestation key on the device.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_GEN_VERSIONED_ATTESTATION_KEY_MINOR_COMMAND \
  18

/**
 * The command to get token information from the device.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_TOKENS_MAJOR_COMMAND 5

/**
 * The command to get the amount of loaded token sets.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_GET_TOKEN_SET_COUNT_MINOR_COMMAND 6

/**
 * The command to get the loaded token at a given set index.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_GET_TOKENS_IN_SET_MINOR_COMMAND 8

/**
 * The command to get the token info at a given set index.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_GET_TOKEN_SET_INFO_MINOR_COMMAND 7

/**
 * The identifier for the getting the major version of the SecurityV2 Info
 * command.
 */
#define HOTH_PRV_CMD_HOTH_SECURITY_V2_INFO_MAJOR_COMMAND 0x0003

// Elliptic Curve signature as per the NIST P256 standard. All fields are
// little-endian with a 32-bit word size. p256_from_le_bin can be used to get a
// usable pair of p256_ints.
struct ec_p256_signature {
  // The r parameter of the EC P256 signature. 32 bytes.
  uint8_t r[32];
  // The s parameter of the EC P256 signature. 32 bytes.
  uint8_t s[32];
};

// Structures used for lightweight passing of allocated buffers/params.
struct libhoth_security_v2_buffer {
  uint8_t* data;
  uint16_t size;
  uint16_t bytes_consumed;
};
struct libhoth_security_v2_param {
  void* data;
  uint16_t size;
};

struct libhoth_security_v2_serialized_param {
  // The number of bytes in this parameter's value.
  uint16_t size;

  // Reserved Bytes
  uint16_t reserved;

  // This parameter's value.
  uint8_t value[];
};

struct libhoth_security_v2_serialized_response_hdr {
  // The number of parameters that follow the header in this response.
  uint16_t param_count;

  // This field ensures that the size of this structure is a multiple of
  // 32-bits, encouraging proper alignment of structures written after
  // it in a byte stream.
  // Its value should never be looked at, but should be set to zero to
  // ensure consistent behavior if the structure is ever signed.
  uint16_t reserved;
};

// Convenience macro for initializing a buffer from allocated storage. Intended
// for passing as a parameter to libhoth_security_v2_exec.
#define LIBHOTH_SECURITY_V2_BUFFER_PARAM(storage)          \
  (&(struct libhoth_security_v2_buffer){.data = (storage), \
                                        .size = sizeof((storage))})

// Convenience macro for initializing a buffer from allocated storage. Intended
// for passing as a parameter to libhoth_security_v2_exec.
#define LIBHOTH_SECURITY_V2_BUFFER_PARAM_WITH_VARIABLE_SIZE(storage,      \
                                                            storage_size) \
  (&(struct libhoth_security_v2_buffer){.data = (storage),                \
                                        .size = (storage_size)})

// Executes a SECURITY_V2 host command with `major.minor` command code. The
// request/response buffers serve as backing storage for the command, and the
// request/response params represent data that will be copied to/from the
// buffers.
int libhoth_security_v2_exec(
    struct libhoth_device* dev, uint8_t major, uint8_t minor,
    uint16_t base_command, struct libhoth_security_v2_buffer* request_buffer,
    const struct libhoth_security_v2_param* request_params,
    uint16_t request_param_count,
    struct libhoth_security_v2_buffer* response_buffer,
    struct libhoth_security_v2_param* response_params,
    uint16_t response_param_count);
// Executes a SECURITY_V2 host command with `major.minor` code, using serialized
// response params
int libhoth_security_v2_exec_serialized(
    struct libhoth_device* dev, uint8_t major, uint8_t minor,
    uint16_t base_command, struct libhoth_security_v2_buffer* request_buffer,
    const struct libhoth_security_v2_param* request_params,
    uint16_t request_param_count,
    struct libhoth_security_v2_buffer* response_buffer,
    const struct libhoth_security_v2_serialized_param** response_params[],
    uint16_t response_param_count);

// Returns the necessary padding given the size
static inline size_t libhoth_security_v2_padding_size(uint16_t size) {
  size_t align = size % sizeof(uint32_t);
  return align == 0 ? 0 : sizeof(uint32_t) - align;
}

#ifdef __cplusplus
}
#endif

#endif  // LIBHOTH_PROTOCOL_SECURITY_V2_H_
