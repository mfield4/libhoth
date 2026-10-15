#include "htool_security_info.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "htool.h"
#include "protocol/rot_firmware_version.h"
#include "protocol/security_info.h"
#include "protocol/security_version.h"

int htool_info(const struct htool_invocation* inv) {
  struct libhoth_device* dev = htool_libhoth_device();
  if (!dev) {
    return -1;
  }

  struct hoth_info info;
  char fw_minor_version[HOTH_INFO_FW_MINOR_VERSION_SIZE + 1];
  memset(fw_minor_version, '\0', HOTH_INFO_FW_MINOR_VERSION_SIZE);

  libhoth_security_version sv = libhoth_get_security_version(dev);
  switch (sv) {
    case LIBHOTH_SECURITY_V2: {
      {
        int status = libhoth_security_info_get(dev, &info);
        if (status != 0) {
          return status;
        }

        struct hoth_response_get_version fw_version_struct;
        libhoth_error err = libhoth_get_rot_fw_version(dev, &fw_version_struct);
        memcpy(&fw_minor_version, &fw_version_struct.version_string_rw,
               sizeof(fw_version_struct.version_string_rw));
        if (err != HOTH_SUCCESS) {
          htool_report_error("get_rot_fw_version", err);
          return -1;
        }
        break;
      }
      // SECURITY_V3 not supported yet.
      default:
        printf("SECURITY_V3 not supported yet\n");
        return -1;
    }
  }
  // Print out the retrieved info.
  printf("hardware_id: 0x%lx\n", info.id.id.hardware_id);
  printf("hardware_category: %d\n", info.id.id.hardware_category);
  printf("bootloader_tag: 0x%x\n", info.id.bootloader_tag);
  printf("fw_epoch: %d\n", info.id.fw_epoch);
  printf("fw_major_version: %d\n", info.id.fw_major_version);
  printf("fw_minor_version: %s\n", fw_minor_version);
  printf("signature_version: %d\n", info.signature_version);
  printf("wrapper_version: %d\n", info.wrapper_version);
  printf("inbound_mailbox_size: %d\n", info.inbound_mailbox_size);
  printf("outbound_mailbox_size: %d\n", info.outbound_mailbox_size);

  return 0;
}
