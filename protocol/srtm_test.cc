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

#include "protocol/srtm.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

#include "protocol/host_cmd.h"
#include "protocol/test/libhoth_device_mock.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;

namespace {

constexpr uint32_t kEmptyResp = 0;

TEST(SrtmRequestFromHexTest, ParsesBytes) {
  struct hoth_srtm_request request = {};
  ASSERT_EQ(libhoth_srtm_request_from_hex(&request, "00a5FF"), 0);
  EXPECT_EQ(request.data_size, 3);
  EXPECT_EQ(request.data[0], 0x00);
  EXPECT_EQ(request.data[1], 0xa5);
  EXPECT_EQ(request.data[2], 0xff);
}

TEST(SrtmRequestFromHexTest, RejectsBadInput) {
  struct hoth_srtm_request request = {};
  EXPECT_EQ(libhoth_srtm_request_from_hex(&request, ""), -1);
  EXPECT_EQ(libhoth_srtm_request_from_hex(&request, "abc"), -1);
  EXPECT_EQ(libhoth_srtm_request_from_hex(&request, "zz"), -1);
  char too_long[SRTM_DATA_MAX_SIZE_BYTES * 2 + 3];
  std::memset(too_long, 'a', sizeof(too_long) - 1);
  too_long[sizeof(too_long) - 1] = '\0';
  EXPECT_EQ(libhoth_srtm_request_from_hex(&request, too_long), -1);
}

MATCHER_P(UsesSrtmRequest, expected, "") {
  const auto* req = static_cast<const struct hoth_host_request*>(arg);
  if (req->command != HOTH_CMD_BOARD_SPECIFIC_BASE + HOTH_PRV_CMD_HOTH_SRTM ||
      req->data_len != sizeof(struct hoth_srtm_request)) {
    return false;
  }
  return std::memcmp(static_cast<const uint8_t*>(arg) +
                         sizeof(struct hoth_host_request),
                     expected, sizeof(struct hoth_srtm_request)) == 0;
}

TEST_F(LibHothTest, SrtmSendsRequest) {
  struct hoth_srtm_request request = {};
  ASSERT_EQ(libhoth_srtm_request_from_hex(&request, "0102"), 0);
  EXPECT_CALL(mock_, send(_, UsesSrtmRequest(&request), _))
      .WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(DoAll(CopyResp(&kEmptyResp, 0), Return(LIBHOTH_OK)));
  EXPECT_EQ(libhoth_srtm(&hoth_dev_, &request), 0);
}

TEST_F(LibHothTest, SrtmSendFailure) {
  struct hoth_srtm_request request = {};
  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_ERR_FAIL));
  EXPECT_NE(libhoth_srtm(&hoth_dev_, &request), 0);
}

}  // namespace
