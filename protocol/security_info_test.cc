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

#include "protocol/security_info.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <vector>

#include "protocol/host_cmd.h"
#include "protocol/security_v2.h"
#include "protocol/test/libhoth_device_mock.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;

namespace {

TEST_F(LibHothTest, SecurityInfoGet) {
  struct hoth_info expected = {};
  expected.id.id.hardware_id = 0x1122334455667788ull;
  expected.id.fw_major_version = 3;
  expected.inbound_mailbox_size = 1024;

  std::vector<uint8_t> resp;
  const struct hoth_security_v2_response_header hdr = {.param_count = 1,
                                                       .reserved = 0};
  resp.insert(resp.end(), (const uint8_t*)&hdr,
              (const uint8_t*)&hdr + sizeof(hdr));
  const struct hoth_security_v2_parameter param = {.size = sizeof(expected),
                                                   .reserved = 0};
  resp.insert(resp.end(), (const uint8_t*)&param,
              (const uint8_t*)&param + sizeof(param));
  resp.insert(resp.end(), (const uint8_t*)&expected,
              (const uint8_t*)&expected + sizeof(expected));
  resp.insert(resp.end(), libhoth_security_v2_padding_size(sizeof(expected)),
              0);

  EXPECT_CALL(
      mock_,
      send(_, UsesCommand(HOTH_BASE_CMD(HOTH_PRV_CMD_HOTH_SECURITY_V2)), _))
      .WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(DoAll(CopyResp(resp.data(), resp.size()), Return(LIBHOTH_OK)));

  struct hoth_info info = {};
  ASSERT_EQ(libhoth_security_info_get(&hoth_dev_, &info), 0);
  EXPECT_EQ(std::memcmp(&info, &expected, sizeof(info)), 0);
}

TEST_F(LibHothTest, SecurityInfoGetSendFailure) {
  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_ERR_FAIL));
  struct hoth_info info = {};
  EXPECT_NE(libhoth_security_info_get(&hoth_dev_, &info), 0);
}

}  // namespace
