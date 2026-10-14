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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "htool.h"
#include "protocol/status.h"
#include "tools/common/htool_cmd.h"
#include "tools/common/macros.h"

static const struct htool_param kCoreGlobalFlags[] = {
    {HTOOL_FLAG_VALUE, .name = "transport", .default_value = "",
     .desc = "The method of connecting to the RoT; for example "
             "'spidev'/'usb'/'mtd'/'dbus'"},
    {HTOOL_FLAG_VALUE, .name = "usb_loc", .default_value = "",
     .desc = "The full bus-portlist location of the RoT; for example "
             "'1-10.4.4.1'."},
    {HTOOL_FLAG_VALUE, .name = "usb_product", .default_value = "",
     .desc = "If there is a single USB RoT with this substring in the USB "
             "product string, use it."},
    {HTOOL_FLAG_VALUE, .name = "spidev_path", .default_value = "",
     .desc = "The full SPIDEV path of the RoT; for example "
             "'/dev/spidev0.0'."},
    {HTOOL_FLAG_BOOL, .name = "spidev_atomic", .default_value = "false",
     .desc = "If true, force spidev to send the request and receive the "
             "corresponding response with a single atomic ioctl.  This is "
             "required on some systems for correctness."},
    {HTOOL_FLAG_VALUE, .name = "spidev_speed_hz", .default_value = "0",
     .desc = "Clock speed (in Hz) to use when using spidev transport. Default "
             "behavior (with input 0) is to not change the clock speed"},
    {HTOOL_FLAG_VALUE, .name = "spidev_device_busy_wait_timeout",
     .default_value = "180000000",
     .desc = "Maximum duration (in microseconds) to wait when SPI device "
             "indicates that it is busy"},
    {HTOOL_FLAG_VALUE, .name = "spidev_device_busy_wait_check_interval",
     .default_value = "100",
     .desc = "Interval duration (in microseconds) to wait before checking SPI "
             "device status again when it indicates that the device is busy"},
    {HTOOL_FLAG_VALUE, .name = "mtddev_path", .default_value = "",
     .desc = "The full MTD path of the RoT mailbox; for example "
             "'/dev/mtd0'. If unspecified, will attempt to detect "
             "the correct device automatically"},
    {HTOOL_FLAG_VALUE, .name = "mtddev_name", .default_value = "hoth-mailbox",
     .desc = "The MTD name of the RoT mailbox; for example 'hoth-mailbox'. "},
    {HTOOL_FLAG_VALUE, .name = "mailbox_location", .default_value = "0",
     .desc = "The location of the mailbox on the RoT, for 'spidev' "
             "or 'mtd' transports; for example '0x900000'."},
    {HTOOL_FLAG_VALUE, .name = "dbus_hoth_id", .default_value = "",
     .desc = "The hoth ID associated with the RoT's hothd service."},
    {HTOOL_FLAG_VALUE, .name = "connect_timeout", .default_value = "10s",
     .desc =
         "Maximum duration to retry opening a busy libhoth transport (e.g., "
         "'1s', '1500ms')."},
    {HTOOL_FLAG_BOOL, .name = "version", .default_value = "false",
     .desc = "Print htool version."},
    {}};

// Built-in transports. Their flags live in kCoreGlobalFlags so that the
// global flag help output keeps its historical order.
static const struct htool_transport kBuiltinTransports[] = {
    {.name = "usb", .open = htool_libhoth_usb_device},
    {.name = "spidev", .open = htool_libhoth_spi_device},
    {.name = "mtd", .open = htool_libhoth_mtd_device},
    {.name = "dbus", .open = htool_libhoth_dbus_device},
};

// Per-run state, reset by htool_run().
static const struct htool_config* g_config;
static struct libhoth_device* g_device;

void htool_report_error(const char* cmd_name, libhoth_error err) {
  if (err == HOTH_SUCCESS) {
    return;
  }

  fprintf(stderr, "Error: '%s' failed (0x%016lX): ", cmd_name, err);
  libhoth_log_err(stderr, err);
}

static const struct htool_transport* find_builtin_transport(const char* name) {
  for (size_t i = 0; i < ARRAY_SIZE(kBuiltinTransports); i++) {
    if (strcmp(kBuiltinTransports[i].name, name) == 0) {
      return &kBuiltinTransports[i];
    }
  }
  return NULL;
}

static const struct htool_transport* find_transport(const char* name) {
  const struct htool_transport* t = find_builtin_transport(name);
  if (t || !g_config->extra_transports) {
    return t;
  }
  for (size_t i = 0; g_config->extra_transports[i]; i++) {
    if (strcmp(g_config->extra_transports[i]->name, name) == 0) {
      return g_config->extra_transports[i];
    }
  }
  return NULL;
}

struct libhoth_device* htool_libhoth_device(void) {
  if (g_device) {
    return g_device;
  }

  const char* transport_method_str;
  int rv = htool_get_param_string(htool_global_flags(), "transport",
                                  &transport_method_str);
  if (rv) {
    return NULL;
  }
  if (transport_method_str[0] == '\0') {
    transport_method_str = "usb";
  }

  const struct htool_transport* t = find_transport(transport_method_str);
  if (!t) {
    fprintf(stderr, "Unknown transport protocol %s\n\r\n",
            transport_method_str);
    return NULL;
  }
  g_device = t->open();
  return g_device;
}

static bool words_equal(const char* const* a, const char* const* b) {
  if (!a || !b) {
    return false;
  }
  for (size_t i = 0;; i++) {
    if (!a[i] || !b[i]) {
      return a[i] == b[i];
    }
    if (strcmp(a[i], b[i]) != 0) {
      return false;
    }
  }
}

static bool cmds_collide(const struct htool_cmd* a, const struct htool_cmd* b) {
  return words_equal(a->verbs, b->verbs) || words_equal(a->verbs, b->alias) ||
         words_equal(a->alias, b->verbs) || words_equal(a->alias, b->alias);
}

static size_t count_params(const struct htool_param* params) {
  size_t n = 0;
  while (params && params[n].type != HTOOL_PARAM_END) {
    n++;
  }
  return n;
}

static size_t count_cmds(const struct htool_cmd* cmds) {
  size_t n = 0;
  while (cmds[n].verbs) {
    n++;
  }
  return n;
}

static int validate_transports(const struct htool_transport* const* extra) {
  for (size_t i = 0; extra && extra[i]; i++) {
    const char* name = extra[i]->name;
    bool dup = !name || find_builtin_transport(name);
    for (size_t j = 0; !dup && j < i; j++) {
      dup = strcmp(extra[j]->name, name) == 0;
    }
    if (dup) {
      fprintf(stderr, "htool: duplicate transport '%s'\n",
              name ? name : "(null)");
      return -1;
    }
  }
  return 0;
}

// Returns a heap-allocated, {}-terminated table of the core global flags
// followed by each extra transport's flags, or NULL on a duplicate.
static struct htool_param* build_global_flags(
    const struct htool_transport* const* extra) {
  size_t n = count_params(kCoreGlobalFlags);
  for (size_t i = 0; extra && extra[i]; i++) {
    n += count_params(extra[i]->flags);
  }
  // struct htool_param has a const member, so fill it with memcpy.
  struct htool_param* flags = calloc(n + 1, sizeof(*flags));
  if (!flags) {
    return NULL;
  }
  size_t len = 0;
  for (const struct htool_param* p = kCoreGlobalFlags; p->type; p++) {
    memcpy(&flags[len++], p, sizeof(*p));
  }
  for (size_t i = 0; extra && extra[i]; i++) {
    for (const struct htool_param* p = extra[i]->flags; p && p->type; p++) {
      memcpy(&flags[len++], p, sizeof(*p));
    }
  }

  for (size_t i = 0; i < len; i++) {
    for (size_t j = 0; j < i; j++) {
      if (strcmp(flags[i].name, flags[j].name) == 0 ||
          (flags[i].ch && flags[i].ch == flags[j].ch)) {
        fprintf(stderr, "htool: duplicate global flag '--%s'\n", flags[i].name);
        free(flags);
        return NULL;
      }
    }
  }
  return flags;
}

static void print_words(FILE* f, const char* const* words) {
  for (size_t i = 0; words[i]; i++) {
    fprintf(f, "%s%s", i ? " " : "", words[i]);
  }
}

// Returns a heap-allocated, {}-terminated table of htool_default_cmds
// followed by each extra table, or NULL on a duplicate.
static struct htool_cmd* build_cmds(const struct htool_cmd* const* extra) {
  size_t n = count_cmds(htool_default_cmds);
  for (size_t i = 0; extra && extra[i]; i++) {
    n += count_cmds(extra[i]);
  }
  struct htool_cmd* cmds = calloc(n + 1, sizeof(*cmds));
  if (!cmds) {
    return NULL;
  }
  size_t len = 0;
  for (const struct htool_cmd* c = htool_default_cmds; c->verbs; c++) {
    cmds[len++] = *c;
  }
  for (size_t i = 0; extra && extra[i]; i++) {
    for (const struct htool_cmd* c = extra[i]; c->verbs; c++) {
      cmds[len++] = *c;
    }
  }

  for (size_t i = 0; i < len; i++) {
    for (size_t j = 0; j < i; j++) {
      if (cmds_collide(&cmds[i], &cmds[j])) {
        fprintf(stderr, "htool: duplicate command '");
        print_words(stderr, cmds[i].verbs);
        fprintf(stderr, "'\n");
        free(cmds);
        return NULL;
      }
    }
  }
  return cmds;
}

int htool_run(const struct htool_config* config, int argc,
              const char* const* argv) {
  g_config = config;
  g_device = NULL;

  if (validate_transports(config->extra_transports)) {
    return -1;
  }
  struct htool_param* flags = build_global_flags(config->extra_transports);
  if (!flags) {
    return -1;
  }
  struct htool_cmd* cmds = build_cmds(config->extra_cmd_tables);
  if (!cmds) {
    free(flags);
    return -1;
  }

  int rv =
      htool_main(flags, cmds, config->version ? config->version : "unknown",
                 argc - 1, argv + 1);
  free(cmds);
  free(flags);
  return rv;
}
