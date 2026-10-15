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

#include "protocol/target_control.h"

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

MATCHER_P2(UsesTargetControl, function, action, "") {
  const auto* req = static_cast<const struct hoth_host_request*>(arg);
  if (req->command !=
          HOTH_CMD_BOARD_SPECIFIC_BASE + HOTH_PRV_CMD_HOTH_TARGET_CONTROL ||
      req->data_len != sizeof(struct hoth_request_target_control)) {
    return false;
  }
  struct hoth_request_target_control payload;
  std::memcpy(
      &payload,
      static_cast<const uint8_t*>(arg) + sizeof(struct hoth_host_request),
      sizeof(payload));
  return payload.function == function && payload.action == action;
}

TEST_F(LibHothTest, TargetControlReturnsStatus) {
  const struct hoth_response_target_control expected = {
      .status = HOTH_TARGET_CONTROL_STATUS_ENABLED};
  EXPECT_CALL(mock_, send(_,
                          UsesTargetControl(HOTH_TARGET_CONTROL_I2C_MUX,
                                            HOTH_TARGET_CONTROL_ACTION_ENABLE),
                          _))
      .WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(
          DoAll(CopyResp(&expected, sizeof(expected)), Return(LIBHOTH_OK)));

  struct hoth_response_target_control response = {};
  ASSERT_EQ(
      libhoth_target_control(&hoth_dev_, HOTH_TARGET_CONTROL_I2C_MUX,
                             HOTH_TARGET_CONTROL_ACTION_ENABLE, &response),
      0);
  EXPECT_EQ(response.status, HOTH_TARGET_CONTROL_STATUS_ENABLED);
}

TEST_F(LibHothTest, TargetControlRejectsShortResponse) {
  const uint8_t short_resp = 0;
  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(
          DoAll(CopyResp(&short_resp, sizeof(short_resp)), Return(LIBHOTH_OK)));

  struct hoth_response_target_control response = {};
  EXPECT_EQ(
      libhoth_target_control(&hoth_dev_, HOTH_TARGET_CONTROL_DEBUG,
                             HOTH_TARGET_CONTROL_ACTION_GET_STATUS, &response),
      -1);
}

TEST_F(LibHothTest, TargetControlSendFailure) {
  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_ERR_FAIL));
  struct hoth_response_target_control response = {};
  EXPECT_NE(
      libhoth_target_control(&hoth_dev_, HOTH_TARGET_CONTROL_DEBUG,
                             HOTH_TARGET_CONTROL_ACTION_GET_STATUS, &response),
      0);
}

TEST_F(LibHothTest, TargetControlNullResponse) {
  EXPECT_EQ(
      libhoth_target_control(&hoth_dev_, HOTH_TARGET_CONTROL_DEBUG,
                             HOTH_TARGET_CONTROL_ACTION_GET_STATUS, nullptr),
      -1);
}

}  // namespace
