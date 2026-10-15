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

#ifndef LIBHOTH_TOOLS_HTOOL_HTOOL_H_
#define LIBHOTH_TOOLS_HTOOL_HTOOL_H_

#include <stddef.h>
#include <stdint.h>

#include "protocol/status.h"
#include "tools/common/htool_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif

void htool_report_error(const char* cmd_name, libhoth_error err);

struct libhoth_device;

struct libhoth_device* htool_libhoth_dbus_device(void);
struct libhoth_device* htool_libhoth_mtd_device(void);
struct libhoth_device* htool_libhoth_spi_device(void);
struct libhoth_device* htool_libhoth_usb_device(void);
// Opens the device selected by the --transport flag, searching the built-in
// transports first and then the extra transports. A non-NULL result is cached
// until the next htool_run() call (the built-in openers also keep their own
// caches).
struct libhoth_device* htool_libhoth_device(void);

// Extension API: lets a custom main() add commands and transports.

struct htool_transport {
  // Value accepted by --transport. Must not collide with a built-in
  // ("usb", "spidev", "mtd", "dbus") or another extra transport.
  const char* name;
  // Extra global flags this transport needs ({}-terminated), or NULL.
  // Read them inside open() via htool_global_flags().
  const struct htool_param* flags;
  // Opens the device. On failure, print a message to stderr and return NULL.
  // Called on first use; a non-NULL result is cached until the next
  // htool_run().
  struct libhoth_device* (*open)(void);
};

struct htool_config {
  // Printed by --version. NULL prints "unknown".
  const char* version;
  // NULL-terminated list of {}-terminated command tables, appended after
  // htool_default_cmds. May be NULL.
  const struct htool_cmd* const* extra_cmd_tables;
  // NULL-terminated list of extra transports. May be NULL.
  const struct htool_transport* const* extra_transports;
};

// Built-in command table ({}-terminated). Exposed for tests and for future
// binaries that want to reuse it.
extern const struct htool_cmd htool_default_cmds[];

// Full entry point. Takes main()'s argc/argv unchanged (argv[0] = program
// name). Returns -1 if the config adds a transport, global flag or command
// that duplicates an existing one. htool_global_flags() is only valid while
// htool_run() is running.
int htool_run(const struct htool_config* config, int argc,
              const char* const* argv);

#ifdef __cplusplus
}
#endif

#endif  // LIBHOTH_TOOLS_HTOOL_HTOOL_H_
