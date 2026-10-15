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

// Template for a custom htool binary that adds its own commands and
// transports on top of the built-in ones, without modifying htool itself.
//
// Build it against //tools/htool:htool_lib (Bazel) or htool_lib (Meson).
// Everything the stock htool supports keeps working; this binary adds:
//
//   htool_extension_example example hello --name=you
//   htool_extension_example --transport example --example_path /dev/foo reboot
//
// Use a prefix other than "htool_" for your own symbols to avoid clashing
// with symbols in htool_lib.

#include <stdio.h>

#include "tools/common/htool_cmd.h"
#include "tools/htool/htool.h"

static int example_cmd_hello(const struct htool_invocation* inv) {
  const char* name;
  if (htool_get_param_string(inv, "name", &name)) {
    return -1;
  }
  printf("Hello, %s!\n", name);
  return 0;
}

static const struct htool_cmd kExampleCmds[] = {
    {
        .verbs = (const char*[]){"example", "hello", NULL},
        .desc = "Print a greeting (extension example).",
        .params =
            (const struct htool_param[]){
                {HTOOL_FLAG_VALUE, .name = "name", .default_value = "world",
                 .desc = "Who to greet."},
                {},
            },
        .func = example_cmd_hello,
    },
    {},
};

// Called by htool_libhoth_device() when --transport=example. A real
// transport would open its device here and return it.
static struct libhoth_device* example_transport_open(void) {
  const char* path;
  if (htool_get_param_string(htool_global_flags(), "example_path", &path)) {
    return NULL;
  }
  fprintf(stderr, "example transport (%s): not a real device\n", path);
  return NULL;
}

static const struct htool_transport kExampleTransport = {
    .name = "example",
    .flags =
        (const struct htool_param[]){
            {HTOOL_FLAG_VALUE, .name = "example_path", .default_value = "",
             .desc = "Device path for the 'example' transport."},
            {},
        },
    .open = example_transport_open,
};

int main(int argc, const char* const* argv) {
  static const struct htool_cmd* const cmds[] = {kExampleCmds, NULL};
  static const struct htool_transport* const transports[] = {&kExampleTransport,
                                                             NULL};
  const struct htool_config config = {
      .version = "htool_extension_example",
      .extra_cmd_tables = cmds,
      .extra_transports = transports,
  };
  return htool_run(&config, argc, argv);
}
