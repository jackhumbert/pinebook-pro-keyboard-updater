#include "updater.h"

int open_usb(int vid, int pid, int indf)
{
  int rc;

  rc = libusb_init(&ctx);
  if(rc < 0)
    return rc;
  
  devintf = indf;

  printf(">>> Trying to open VID:%04x PID:%04x...\n", vid&0xffff, pid&0xffff);
  devh = libusb_open_device_with_vid_pid(ctx, vid, pid);
  if(devh == NULL) {
    printf(">>> USB device not found\n");
    return -1;
  }

  rc = libusb_kernel_driver_active(devh, indf);
  if (rc > 0) {
    printf(">>> Kernel driver active\n");
    rc = libusb_detach_kernel_driver(devh, indf);
    if (rc < 0) {
      printf(">>> libusb_detach_kernel_driver: %d\n", rc);
      goto finish;
    }
  }

  rc = libusb_claim_interface(devh, indf);
  if(rc < 0) {
    printf(">>> libusb_claim_interface: %d\n", rc);
    goto finish;
  }

finish:
  if (rc < 0) {
    close_usb();
  }
  return rc;
}

void close_usb()
{
  if (devh) {
    printf(">>> USB device closed\n");
    libusb_release_interface(devh, devintf);
    libusb_close(devh);
    libusb_exit(ctx);
    devh = NULL;
  }
}

int open_user_mode()
{
  int rc = open_usb(0x258a, 0x001e, 1);
  if (rc < 0) {
    rc = open_usb(0x258a, 0x001f, 1);
  }
  if (rc < 0) {
    rc = open_usb(0x258a, 0x000d, 1);
  }

  return rc;
}

int open_touchpad_mode()
{
  int rc = open_usb(0x258a, 0x001f, 1);
  if (rc < 0) {
    rc = open_usb(0x258a, 0x000d, 1);
  }

  return rc;
}

int open_boot_mode()
{
  return open_usb(0x0603, 0x1020, 0);
}

int switch_to_boot_mode()
{
  int rc, try;

  printf("[*] Opening USB device in user mode...\n");
  for (try = 0; try < 3; try++) {
    rc = open_user_mode();
    if (rc >= 0) {
      break;
    }
    usleep(500*1000);
  }

  if (try == 3) {
    printf("EEE Failed to open in user mode\n");
    goto finish;
  }

  printf("[*] Sending command to switch to boot mode...\n");

  unsigned char dataOut[6] = {
    0x5, 0x75
  };
  rc = libusb_control_transfer(devh, 0x21, 0x09, 0x0305, 1,
    dataOut, sizeof(dataOut), 1000);
  if (rc < 0) {
    printf("EEE Failed to send switch command\n");
    goto finish;
  }

  printf(">>> Switch command sent\n");

finish:
  close_usb();
  return rc;
}

int reset_device()
{
  int rc;

  unsigned char data[6] = {
    0x05, 0x55, 0x55, 0x55, 0x55, 0x55
  };

  rc = libusb_control_transfer(devh, 0x21, 0x09, 0x0305, 0, data, sizeof(data), 100);
  if (rc < 0) {
    return rc;
  }

  rc = libusb_reset_device(devh);
  if (rc < 0) {
    return rc;
  }

  return 0;
}
