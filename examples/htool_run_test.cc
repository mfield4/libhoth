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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "htool.h"
#include "htool_cmd.h"

namespace {

using ::testing::HasSubstr;
using ::testing::Not;

int g_calls = 0;
int g_fake_device_storage = 0;
libhoth_device* const kFakeDevice =
    reinterpret_cast<libhoth_device*>(&g_fake_device_storage);

const htool_param kNoParams[] = {{}};

int CountCall(const htool_invocation*) {
  g_calls++;
  return 0;
}

int CheckFakeDevice(const htool_invocation*) {
  g_calls++;
  EXPECT_EQ(htool_libhoth_device(), kFakeDevice);
  const char* path = nullptr;
  EXPECT_EQ(htool_get_param_string(htool_global_flags(), "fake_path", &path),
            0);
  EXPECT_STREQ(path, "x");
  return 0;
}

libhoth_device* OpenFake() { return kFakeDevice; }

const char* const kPingVerbs[] = {"ext", "ping", nullptr};
const char* const kDevVerbs[] = {"ext", "dev", nullptr};
const char* const kPayloadInfoExtVerbs[] = {"payload", "info", "ext", nullptr};
const char* const kRebootVerbs[] = {"reboot", nullptr};

const htool_cmd kExtCmds[] = {
    {kPingVerbs, nullptr, "ping", kNoParams, CountCall, nullptr},
    {kDevVerbs, nullptr, "dev", kNoParams, CheckFakeDevice, nullptr},
    {kPayloadInfoExtVerbs, nullptr, "payload info ext", kNoParams, CountCall,
     nullptr},
    {},
};
const htool_cmd* const kExtCmdTables[] = {kExtCmds, nullptr};

const htool_param kFakeFlags[] = {
    {HTOOL_FLAG_VALUE, 0, "fake_path", "", "Path for the fake transport."},
    {},
};
const htool_transport kFakeTransport = {"fake", kFakeFlags, OpenFake};
const htool_transport* const kFakeTransports[] = {&kFakeTransport, nullptr};

template <size_t N>
int RunHtool(const htool_config& config, const char* const (&argv)[N]) {
  return htool_run(&config, N, argv);
}

class HtoolRunTest : public ::testing::Test {
 protected:
  void SetUp() override { g_calls = 0; }
};

TEST_F(HtoolRunTest, DispatchesExtraCommand) {
  htool_config config = {};
  config.extra_cmd_tables = kExtCmdTables;
  const char* const argv[] = {"htool", "ext", "ping"};
  EXPECT_EQ(RunHtool(config, argv), 0);
  EXPECT_EQ(g_calls, 1);
}

TEST_F(HtoolRunTest, ExtraTransportAndFlag) {
  htool_config config = {};
  config.extra_cmd_tables = kExtCmdTables;
  config.extra_transports = kFakeTransports;
  const char* const argv[] = {"htool", "--transport", "fake", "--fake_path",
                              "x",     "ext",         "dev"};
  EXPECT_EQ(RunHtool(config, argv), 0);
  EXPECT_EQ(g_calls, 1);
}

TEST_F(HtoolRunTest, RejectsTransportNamedLikeBuiltin) {
  const htool_transport usb = {"usb", nullptr, OpenFake};
  const htool_transport* const transports[] = {&usb, nullptr};
  htool_config config = {};
  config.extra_cmd_tables = kExtCmdTables;
  config.extra_transports = transports;
  const char* const argv[] = {"htool", "ext", "ping"};
  testing::internal::CaptureStderr();
  EXPECT_NE(RunHtool(config, argv), 0);
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              HasSubstr("duplicate transport 'usb'"));
  EXPECT_EQ(g_calls, 0);
}

TEST_F(HtoolRunTest, RejectsDuplicateGlobalFlag) {
  const htool_param flags[] = {
      {HTOOL_FLAG_VALUE, 0, "usb_loc", "", "Clashes with a core flag."},
      {},
  };
  const htool_transport clash = {"clash", flags, OpenFake};
  const htool_transport* const transports[] = {&clash, nullptr};
  htool_config config = {};
  config.extra_cmd_tables = kExtCmdTables;
  config.extra_transports = transports;
  const char* const argv[] = {"htool", "ext", "ping"};
  testing::internal::CaptureStderr();
  EXPECT_NE(RunHtool(config, argv), 0);
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              HasSubstr("duplicate global flag '--usb_loc'"));
  EXPECT_EQ(g_calls, 0);
}

TEST_F(HtoolRunTest, RejectsDuplicateCommand) {
  const htool_cmd cmds[] = {
      {kRebootVerbs, nullptr, "reboot", kNoParams, CountCall, nullptr},
      {},
  };
  const htool_cmd* const tables[] = {cmds, nullptr};
  htool_config config = {};
  config.extra_cmd_tables = tables;
  const char* const argv[] = {"htool", "reboot"};
  testing::internal::CaptureStderr();
  EXPECT_NE(RunHtool(config, argv), 0);
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              HasSubstr("duplicate command 'reboot'"));
  EXPECT_EQ(g_calls, 0);
}

TEST_F(HtoolRunTest, LongestMatchBeatsDefaultPrefix) {
  htool_config config = {};
  config.extra_cmd_tables = kExtCmdTables;
  const char* const argv[] = {"htool", "payload", "info", "ext"};
  EXPECT_EQ(RunHtool(config, argv), 0);
  EXPECT_EQ(g_calls, 1);
}

TEST_F(HtoolRunTest, PrintsConfiguredVersion) {
  htool_config config = {};
  config.version = "test-version-123";
  const char* const argv[] = {"htool", "--version"};
  testing::internal::CaptureStdout();
  EXPECT_EQ(RunHtool(config, argv), 0);
  EXPECT_EQ(testing::internal::GetCapturedStdout(), "test-version-123\n");
}

TEST_F(HtoolRunTest, DefaultTablePassesValidation) {
  htool_config config = {};
  const char* const argv[] = {"htool", "ext-nonexistent"};
  testing::internal::CaptureStderr();
  EXPECT_EQ(RunHtool(config, argv), -1);
  std::string err = testing::internal::GetCapturedStderr();
  EXPECT_THAT(err, HasSubstr("Unknown subcommand"));
  EXPECT_THAT(err, Not(HasSubstr("duplicate")));
}

}  // namespace
