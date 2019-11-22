#include "updater.h"

libusb_device_handle *devh;
libusb_context *ctx;

int main(int argc, char *argv[])
{
  int rc = 0;

  rc = write_kb_fw("files/tmp.hex");
  if (rc < 0) {
   return rc;
  }

  rc = write_tp_fw("files/tpfw.bin");
  if (rc < 0) {
    return rc;
  }

  rc = switch_to_boot_mode();

  rc = write_kb_fw("files/fw.hex");
  if (rc < 0) {
    return rc;
  }

  return rc;
}
