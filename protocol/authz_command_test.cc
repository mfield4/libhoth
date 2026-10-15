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

#include "protocol/authz_command.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

namespace {

TEST(AuthzCommandTest, BuildRequestFillsFields) {
  uint32_t nonce[AUTHORIZED_COMMAND_NONCE_SIZE / sizeof(uint32_t)];
  for (size_t i = 0; i < sizeof(nonce) / sizeof(nonce[0]); i++) {
    nonce[i] = 0x01010101u * (i + 1);
  }

  struct hoth_authorized_command_request request =
      libhoth_authz_command_build_request(0x1122334455667788ull,
                                          /*opcode=*/0x42, /*key_info=*/7,
                                          nonce);

  EXPECT_EQ(request.version, AUTHORIZED_COMMAND_VERSION);
  EXPECT_EQ(request.size, sizeof(request));
  EXPECT_EQ(request.key_info, 7u);
  EXPECT_EQ(request.dev_id_0, 0x55667788u);
  EXPECT_EQ(request.dev_id_1, 0x11223344u);
  EXPECT_EQ(request.opcode, 0x42u);
  EXPECT_EQ(std::memcmp(request.nonce, nonce, sizeof(nonce)), 0);
  for (uint8_t b : request.signature) {
    EXPECT_EQ(b, 0);
  }
}

}  // namespace
