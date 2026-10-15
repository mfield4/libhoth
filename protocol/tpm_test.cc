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

#include "protocol/tpm.h"

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

MATCHER_P(UsesSetTpmMode, mode, "") {
  const auto* req = static_cast<const struct hoth_host_request*>(arg);
  if (req->command !=
          HOTH_CMD_BOARD_SPECIFIC_BASE + HOTH_PRV_CMD_SET_TPM_MODE ||
      req->data_len != sizeof(struct tpm_mode)) {
    return false;
  }
  struct tpm_mode payload;
  std::memcpy(
      &payload,
      static_cast<const uint8_t*>(arg) + sizeof(struct hoth_host_request),
      sizeof(payload));
  return payload.mode == mode;
}

TEST_F(LibHothTest, TpmSetMode) {
  EXPECT_CALL(mock_, send(_, UsesSetTpmMode(TPM_MODE_SPI_NOR_MAILBOX), _))
      .WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(DoAll(CopyResp(&kEmptyResp, 0), Return(LIBHOTH_OK)));
  EXPECT_EQ(libhoth_tpm_set_mode(&hoth_dev_, TPM_MODE_SPI_NOR_MAILBOX), 0);
}

TEST_F(LibHothTest, TpmGetMode) {
  const struct tpm_mode expected = {.mode = TPM_MODE_TPM_SPI};
  EXPECT_CALL(mock_, send(_,
                          UsesCommand(HOTH_CMD_BOARD_SPECIFIC_BASE +
                                      HOTH_PRV_CMD_GET_TPM_MODE),
                          _))
      .WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(
          DoAll(CopyResp(&expected, sizeof(expected)), Return(LIBHOTH_OK)));

  struct tpm_mode mode = {};
  ASSERT_EQ(libhoth_tpm_get_mode(&hoth_dev_, &mode), 0);
  EXPECT_EQ(mode.mode, TPM_MODE_TPM_SPI);
}

TEST_F(LibHothTest, TpmGetModeSendFailure) {
  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_ERR_FAIL));
  struct tpm_mode mode = {};
  EXPECT_NE(libhoth_tpm_get_mode(&hoth_dev_, &mode), 0);
}

}  // namespace
