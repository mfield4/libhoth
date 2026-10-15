#include "protocol/security_v2.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "protocol/host_cmd.h"
#include "protocol/test/libhoth_device_mock.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;

class SecurityV2Test : public LibHothTest {};

TEST_F(SecurityV2Test, ExecCmdSuccess) {
  uint8_t request_storage[128];
  uint8_t response_storage[128];
  struct libhoth_security_v2_buffer request_buffer = {
      .data = request_storage, .size = sizeof(request_storage)};
  struct libhoth_security_v2_buffer response_buffer = {
      .data = response_storage, .size = sizeof(response_storage)};

  // Request
  uint32_t req_param_val = 0xdeadbeef;
  struct libhoth_security_v2_param request_params[] = {
      {.data = &req_param_val, .size = sizeof(req_param_val)}};

  // Response
  uint8_t resp_param_val[] = {1, 2, 3, 4, 5};
  uint8_t resp_param_buf[sizeof(resp_param_val)] = {0};
  struct libhoth_security_v2_param response_params[] = {
      {.data = resp_param_buf, .size = sizeof(resp_param_buf)}};

  // Construct expected response from device
  std::vector<uint8_t> device_response;
  struct hoth_security_v2_response_header resp_hdr = {.param_count = 1,
                                                      .reserved = 0};
  device_response.insert(device_response.end(), (uint8_t*)&resp_hdr,
                         (uint8_t*)&resp_hdr + sizeof(resp_hdr));

  struct hoth_security_v2_parameter resp_param_hdr = {
      .size = sizeof(resp_param_val), .reserved = 0};
  device_response.insert(device_response.end(), (uint8_t*)&resp_param_hdr,
                         (uint8_t*)&resp_param_hdr + sizeof(resp_param_hdr));
  device_response.insert(device_response.end(), resp_param_val,
                         resp_param_val + sizeof(resp_param_val));
  size_t padding = libhoth_security_v2_padding_size(sizeof(resp_param_val));
  ASSERT_EQ(padding, 3);
  device_response.insert(device_response.end(), padding, 0);

  EXPECT_CALL(mock_, send(&hoth_dev_, UsesCommand(3), _))
      .WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(DoAll(CopyResp(device_response.data(), device_response.size()),
                      Return(LIBHOTH_OK)));

  int status = libhoth_security_v2_exec(&hoth_dev_, 1, 2, 3, &request_buffer,
                                        request_params, 1, &response_buffer,
                                        response_params, 1);

  ASSERT_EQ(status, 0);
  EXPECT_EQ(memcmp(resp_param_buf, resp_param_val, sizeof(resp_param_val)), 0);
}

TEST_F(SecurityV2Test, ExecCmdHostCmdFailure) {
  uint8_t request_storage[128];
  uint8_t response_storage[128];
  struct libhoth_security_v2_buffer request_buffer = {
      .data = request_storage, .size = sizeof(request_storage)};
  struct libhoth_security_v2_buffer response_buffer = {
      .data = response_storage, .size = sizeof(response_storage)};

  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_ERR_FAIL));

  int status =
      libhoth_security_v2_exec(&hoth_dev_, 1, 2, 3, &request_buffer, nullptr, 0,
                               &response_buffer, nullptr, 0);

  ASSERT_EQ(status, -1);
}

TEST_F(SecurityV2Test, SerializedCmdSuccess) {
  uint8_t request_storage[128];
  uint8_t response_storage[128];
  struct libhoth_security_v2_buffer request_buffer = {
      .data = request_storage, .size = sizeof(request_storage)};
  struct libhoth_security_v2_buffer response_buffer = {
      .data = response_storage, .size = sizeof(response_storage)};

  // Construct expected response
  std::vector<uint8_t> response;
  struct libhoth_security_v2_serialized_response_hdr hdr = {.param_count = 2,
                                                            .reserved = 0};
  response.insert(response.end(), (uint8_t*)&hdr, (uint8_t*)&hdr + sizeof(hdr));

  // Param 1
  uint32_t param1_val = 0xdeadbeef;
  struct libhoth_security_v2_serialized_param param1_hdr = {
      .size = sizeof(param1_val), .reserved = 0};
  response.insert(response.end(), (uint8_t*)&param1_hdr,
                  (uint8_t*)&param1_hdr + sizeof(param1_hdr));
  response.insert(response.end(), (uint8_t*)&param1_val,
                  (uint8_t*)&param1_val + sizeof(param1_val));
  // No padding for size 4

  // Param 2
  uint8_t param2_val[] = {1, 2, 3, 4, 5};
  struct libhoth_security_v2_serialized_param param2_hdr = {
      .size = sizeof(param2_val), .reserved = 0};
  response.insert(response.end(), (uint8_t*)&param2_hdr,
                  (uint8_t*)&param2_hdr + sizeof(param2_hdr));
  response.insert(response.end(), param2_val, param2_val + sizeof(param2_val));
  size_t padding2 = libhoth_security_v2_padding_size(sizeof(param2_val));
  ASSERT_EQ(padding2, 3);
  response.insert(response.end(), padding2, 0);

  EXPECT_CALL(mock_,
              send(&hoth_dev_, UsesCommand(3),
                   sizeof(struct hoth_host_request) + request_buffer.size))
      .WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(DoAll(CopyResp(response.data(), response.size()),
                      Return(LIBHOTH_OK)));

  const struct libhoth_security_v2_serialized_param* param_out1 = nullptr;
  const struct libhoth_security_v2_serialized_param* param_out2 = nullptr;
  const struct libhoth_security_v2_serialized_param** response_params[] = {
      &param_out1, &param_out2};

  int status = libhoth_security_v2_exec_serialized(
      &hoth_dev_, 1, 2, 3, &request_buffer, nullptr, 0, &response_buffer,
      response_params, 2);

  ASSERT_EQ(status, 0);
  ASSERT_NE(param_out1, nullptr);
  EXPECT_EQ(param_out1->size, sizeof(param1_val));
  EXPECT_EQ(memcmp(param_out1->value, &param1_val, sizeof(param1_val)), 0);

  ASSERT_NE(param_out2, nullptr);
  EXPECT_EQ(param_out2->size, sizeof(param2_val));
  EXPECT_EQ(memcmp(param_out2->value, param2_val, sizeof(param2_val)), 0);
}

TEST_F(SecurityV2Test, SerializedCmdExecFailure) {
  uint8_t request_storage[128];
  uint8_t response_storage[128];
  struct libhoth_security_v2_buffer request_buffer = {
      .data = request_storage, .size = sizeof(request_storage)};
  struct libhoth_security_v2_buffer response_buffer = {
      .data = response_storage, .size = sizeof(response_storage)};

  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_ERR_FAIL));

  const struct libhoth_security_v2_serialized_param* param_out1 = nullptr;
  const struct libhoth_security_v2_serialized_param** response_params[] = {
      &param_out1};

  int status = libhoth_security_v2_exec_serialized(
      &hoth_dev_, 1, 2, 3, &request_buffer, nullptr, 0, &response_buffer,
      response_params, 1);

  ASSERT_EQ(status, -1);
}

TEST_F(SecurityV2Test, SerializedCmdWrongParamCount) {
  uint8_t request_storage[128];
  uint8_t response_storage[128];
  struct libhoth_security_v2_buffer request_buffer = {
      .data = request_storage, .size = sizeof(request_storage)};
  struct libhoth_security_v2_buffer response_buffer = {
      .data = response_storage, .size = sizeof(response_storage)};

  // Construct response with param_count = 1
  std::vector<uint8_t> response;
  struct libhoth_security_v2_serialized_response_hdr hdr = {.param_count = 1,
                                                            .reserved = 0};
  response.insert(response.end(), (uint8_t*)&hdr, (uint8_t*)&hdr + sizeof(hdr));

  uint32_t param1_val = 0xdeadbeef;
  struct libhoth_security_v2_serialized_param param1_hdr = {
      .size = sizeof(param1_val), .reserved = 0};
  response.insert(response.end(), (uint8_t*)&param1_hdr,
                  (uint8_t*)&param1_hdr + sizeof(param1_hdr));
  response.insert(response.end(), (uint8_t*)&param1_val,
                  (uint8_t*)&param1_val + sizeof(param1_val));
  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(DoAll(CopyResp(response.data(), response.size()),
                      Return(LIBHOTH_OK)));

  const struct libhoth_security_v2_serialized_param* param_out1 = nullptr;
  const struct libhoth_security_v2_serialized_param* param_out2 = nullptr;
  // We expect 2 params
  const struct libhoth_security_v2_serialized_param** response_params[] = {
      &param_out1, &param_out2};

  int status = libhoth_security_v2_exec_serialized(
      &hoth_dev_, 1, 2, 3, &request_buffer, nullptr, 0, &response_buffer,
      response_params, 2);

  ASSERT_EQ(status, -1);
}

TEST_F(SecurityV2Test, SerializedCmdBadPadding) {
  uint8_t request_storage[128];
  uint8_t response_storage[128];
  struct libhoth_security_v2_buffer request_buffer = {
      .data = request_storage, .size = sizeof(request_storage)};
  struct libhoth_security_v2_buffer response_buffer = {
      .data = response_storage, .size = sizeof(response_storage)};

  // Construct response
  std::vector<uint8_t> response;
  struct libhoth_security_v2_serialized_response_hdr hdr = {.param_count = 1,
                                                            .reserved = 0};
  response.insert(response.end(), (uint8_t*)&hdr, (uint8_t*)&hdr + sizeof(hdr));

  // Param with non-zero padding
  uint8_t param1_val[] = {1, 2, 3, 4, 5};
  struct libhoth_security_v2_serialized_param param1_hdr = {
      .size = sizeof(param1_val), .reserved = 0};
  response.insert(response.end(), (uint8_t*)&param1_hdr,
                  (uint8_t*)&param1_hdr + sizeof(param1_hdr));
  response.insert(response.end(), param1_val, param1_val + sizeof(param1_val));
  size_t padding1 = libhoth_security_v2_padding_size(sizeof(param1_val));
  ASSERT_EQ(padding1, 3);
  // Add bad padding
  response.push_back(0);
  response.push_back(1);  // non-zero
  response.push_back(0);

  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(DoAll(CopyResp(response.data(), response.size()),
                      Return(LIBHOTH_OK)));

  const struct libhoth_security_v2_serialized_param* param_out1 = nullptr;
  const struct libhoth_security_v2_serialized_param** response_params[] = {
      &param_out1};

  int status = libhoth_security_v2_exec_serialized(
      &hoth_dev_, 1, 2, 3, &request_buffer, nullptr, 0, &response_buffer,
      response_params, 1);

  ASSERT_EQ(status, -1);
}

TEST_F(SecurityV2Test, SerializedCmdInsufficientBytesForValue) {
  uint8_t request_storage[128];
  uint8_t response_storage[128];
  struct libhoth_security_v2_buffer request_buffer = {
      .data = request_storage, .size = sizeof(request_storage)};
  struct libhoth_security_v2_buffer response_buffer = {
      .data = response_storage, .size = sizeof(response_storage)};

  // Construct response
  std::vector<uint8_t> response;
  struct libhoth_security_v2_serialized_response_hdr hdr = {.param_count = 1,
                                                            .reserved = 0};
  response.insert(response.end(), (uint8_t*)&hdr, (uint8_t*)&hdr + sizeof(hdr));

  // Param with size larger than available data
  uint32_t param1_val = 0xdeadbeef;
  struct libhoth_security_v2_serialized_param param1_hdr = {
      .size = sizeof(param1_val), .reserved = 0};
  response.insert(response.end(), (uint8_t*)&param1_hdr,
                  (uint8_t*)&param1_hdr + sizeof(param1_hdr));
  // Only add 2 bytes of value instead of 4
  response.push_back(0xef);
  response.push_back(0xbe);

  EXPECT_CALL(mock_, send(_, _, _)).WillOnce(Return(LIBHOTH_OK));
  EXPECT_CALL(mock_, receive)
      .WillOnce(DoAll(CopyResp(response.data(), response.size()),
                      Return(LIBHOTH_OK)));

  const struct libhoth_security_v2_serialized_param* param_out1 = nullptr;
  const struct libhoth_security_v2_serialized_param** response_params[] = {
      &param_out1};

  int status = libhoth_security_v2_exec_serialized(
      &hoth_dev_, 1, 2, 3, &request_buffer, nullptr, 0, &response_buffer,
      response_params, 1);

  ASSERT_EQ(status, -1);
}
