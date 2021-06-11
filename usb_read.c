#include "updater.h"

static int read_block_start(int length)
{
  unsigned char transfer[6];

  printf(">>> USB read_block_start length:%d\n", length);

  transfer[0] = 0x05;//report id
  transfer[1] = 0x52;
  transfer[2] = 0x00;
  transfer[3] = 0x00;
  transfer[4] = length & 0xFF;
  transfer[5] = (length >> 8) & 0xFF;

  return libusb_control_transfer(devh, 0x21, 0x09, 0x0305, 0,
    transfer, sizeof(transfer), 100);
}

static int read_block(unsigned char *data, int offset, int length)
{
  unsigned char transfer[length + 2];

  printf(">>> USB read_block offset:%d length:%d\n", offset, length);

  transfer[0] = 0x06;//report id
  transfer[1] = 0x72;

  int rc = libusb_control_transfer(devh, 0xa1, 0x01, 0x0306, 0,
    transfer, sizeof(transfer), 2000);

  if (rc < 0) {
    return rc;
  }

  memcpy(&data[offset], &transfer[2], length);

  return 0;
}

int read_bulk(unsigned char *data, int length)
{
  int block_size = 2048;
  int rc;

  rc = read_block_start(length);
  if (rc < 0) {
    return rc;
  }

  for (int offset = 0; offset < length; offset += block_size) {
    rc = read_block(data, offset, block_size);
    if (rc < 0) {
      return rc;
    }

    usleep(10000);
  }

  return 0;
}
