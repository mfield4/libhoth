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

#include "protocol/spi_passthrough.h"

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

TEST_F(LibHothTest, SpiPassthroughEnable) {
  EXPECT_CALL(mock_, send(_,
                          UsesCommand(HOTH_CMD_BOARD_SPECIFIC_BASE +
                                      HOTH_PRV_CMD_HOTH_SPS_PASSTHROUGH_ENABLE),
                          sizeof(struct hoth_host_request)))
      .WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(DoAll(CopyResp(&kEmptyResp, 0), Return(LIBHOTH_OK)));
  EXPECT_EQ(libhoth_spi_passthrough_set(&hoth_dev_, true), 0);
}

TEST_F(LibHothTest, SpiPassthroughDisable) {
  EXPECT_CALL(mock_,
              send(_,
                   UsesCommand(HOTH_CMD_BOARD_SPECIFIC_BASE +
                               HOTH_PRV_CMD_HOTH_SPS_PASSTHROUGH_DISABLE),
                   sizeof(struct hoth_host_request)))
      .WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(DoAll(CopyResp(&kEmptyResp, 0), Return(LIBHOTH_OK)));
  EXPECT_EQ(libhoth_spi_passthrough_set(&hoth_dev_, false), 0);
}

TEST_F(LibHothTest, SpiPassthroughSendFailure) {
  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_ERR_FAIL));
  EXPECT_NE(libhoth_spi_passthrough_set(&hoth_dev_, true), 0);
}

TEST_F(LibHothTest, FlashSpiInfo) {
  const struct hoth_response_flash_spi_info expected = {
      .jedec = {0xef, 0x40, 0x19},
      .reserved0 = 0,
      .mfr_dev_id = {0xef, 0x18},
      .sr1 = 0x02,
      .sr2 = 0x03,
  };
  EXPECT_CALL(mock_, send(_, UsesCommand(HOTH_CMD_FLASH_SPI_INFO), _))
      .WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(
          DoAll(CopyResp(&expected, sizeof(expected)), Return(LIBHOTH_OK)));

  struct hoth_response_flash_spi_info info = {};
  ASSERT_EQ(libhoth_flash_spi_info(&hoth_dev_, &info), 0);
  EXPECT_EQ(std::memcmp(&info, &expected, sizeof(info)), 0);
}

TEST_F(LibHothTest, FlashSpiInfoSendFailure) {
  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_ERR_FAIL));
  struct hoth_response_flash_spi_info info = {};
  EXPECT_NE(libhoth_flash_spi_info(&hoth_dev_, &info), 0);
}

}  // namespace
