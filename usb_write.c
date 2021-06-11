#include "updater.h"

static int write_block_start(int length)
{
  unsigned char transfer[6];

  printf(">>> USB write_block_start length:%d\n", length);

  transfer[0] = 0x05;//report id
  transfer[1] = 0x57;
  transfer[2] = 0x00;
  transfer[3] = 0x00;
  transfer[4] = length & 0xFF;
  transfer[5] = (length >> 8) & 0xFF;

  return libusb_control_transfer(devh, 0x21, 0x09, 0x0305, 0,
    transfer, sizeof(transfer), 100);
}

static int write_block(unsigned char *data, int offset, int length)
{
  unsigned char transfer[length + 2];

  printf(">>> USB write_block offset:%d length:%d\n", offset, length);

  transfer[0] = 0x06;//report id
  transfer[1] = 0x77;

  memcpy(&transfer[2], &data[offset], length);

  return libusb_control_transfer(devh, 0x21, 0x09, 0x0306, 0,
    transfer, sizeof(transfer), 2000);
}

int write_bulk(unsigned char *data, int length)
{
  int block_size = 2048;
  int rc;

  rc = write_block_start(length);
  if (rc < 0) {
    return rc;
  }

  // HACK: overwrite first byte (as in original sources)
  unsigned char first_byte = data[0];
  data[0] = 0;

  for (int offset = 0; offset < length; offset += block_size) {
    rc = write_block(data, offset, block_size);
    if (rc < 0) {
      return rc;
    }

    usleep(10000);
  }

  data[0] = first_byte;

  // retry write of first block
  rc = write_block_start(length);
  if (rc < 0) {
    return rc;
  }

  rc = write_block(data, 0, block_size);
  if (rc < 0) {
    return rc;
  }

  return 0;
}
