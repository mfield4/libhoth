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

#include "protocol/target_reset.h"

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

MATCHER_P(UsesResetOption, option, "") {
  const auto* req = static_cast<const struct hoth_host_request*>(arg);
  if (req->command !=
          HOTH_CMD_BOARD_SPECIFIC_BASE + HOTH_PRV_CMD_HOTH_RESET_TARGET ||
      req->data_len != sizeof(struct hoth_request_reset_target)) {
    return false;
  }
  struct hoth_request_reset_target payload;
  std::memcpy(
      &payload,
      static_cast<const uint8_t*>(arg) + sizeof(struct hoth_host_request),
      sizeof(payload));
  return payload.target_id == RESET_TARGET_ID_RSTCTRL0 &&
         payload.reset_option == option;
}

TEST_F(LibHothTest, TargetResetSendsOption) {
  for (auto option :
       {HOTH_TARGET_RESET_OPTION_RELEASE, HOTH_TARGET_RESET_OPTION_SET,
        HOTH_TARGET_RESET_OPTION_PULSE}) {
    EXPECT_CALL(mock_, send(_, UsesResetOption(option), _))
        .WillOnce(Return(LIBHOTH_OK));
    EXPECT_CALL(mock_, receive)
        .WillOnce(DoAll(CopyResp(&kEmptyResp, 0), Return(LIBHOTH_OK)));
    EXPECT_EQ(libhoth_target_reset(&hoth_dev_, option), 0);
  }
}

TEST_F(LibHothTest, TargetResetSendFailure) {
  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_ERR_FAIL));
  EXPECT_NE(libhoth_target_reset(&hoth_dev_, HOTH_TARGET_RESET_OPTION_SET), 0);
}

TEST_F(LibHothTest, ArmCoordinatedReset) {
  EXPECT_CALL(mock_, send(_,
                          UsesCommand(HOTH_CMD_BOARD_SPECIFIC_BASE +
                                      HOTH_PRV_CMD_HOTH_ARM_COORDINATED_RESET),
                          sizeof(struct hoth_host_request)))
      .WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(DoAll(CopyResp(&kEmptyResp, 0), Return(LIBHOTH_OK)));
  EXPECT_EQ(libhoth_arm_coordinated_reset(&hoth_dev_), 0);
}

}  // namespace
