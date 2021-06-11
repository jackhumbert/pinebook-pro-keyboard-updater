#include "updater.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define SHORTREPOID			0x05
#define LONGREPOID			0x06
#define SHORTDEVLENGHT		6
#define LONGDEVLENGHT		1040


#define STATUSCMD				  0xA1


#define CHECKCHECKSUM             0xF0
#define ENTERBOOTLOADER           0xF1
#define ICERASE                   0xF2
#define PROGRAM                   0xF3
#define VERIFY1KDATA              0xF4
#define VERIFY_CHECKSUM           0xF5
#define PROGRAMPASS               0xF6
#define ENDPROGRAM                0xF7
#define I2C_PASS                  0xFA
#define I2C_FAIL                  0xFB
#define I2C_WAIT                  0xFC
#define I2C_TIMEOUT               0xFD

#define CHECKCHECKSUM_PASS              0xE0
#define ENTERBOOTLOADER_PASS            0xE1
#define ICERASE_PASS                    0xE2
#define PROGRAM_PASS                    0xE3
#define VERIFY1KDATA_PASS               0xE4
#define VERIFY_CHECKSUM_PASS            0xE5
#define PROGRAMPASS_PASS                0xE6
#define ENDPROGRAM_PASS                 0xE7

#define CHECKCHECKSUM_FAIL              0xD0
#define ENTERBOOTLOADER_FAIL            0xD1
#define ICERASE_FAIL                    0xD2
#define PROGRAM_FAIL                    0xD3
#define VERIFY1KDATA_FAIL               0xD4
#define VERIFY_CHECKSUM_FAIL            0xD5
#define PROGRAMPASS_FAIL                0xD6
#define ENDPROGRAM_FAIL                 0xD7

#define PROGMODE				  0x0b

static int touchpad_verify(int type, int pass, int sendcmd)
{
    if (sendcmd) {
        unsigned char data[6] = {
            SHORTREPOID, STATUSCMD, type
        };

        int rc = libusb_control_transfer(devh, 0x21, 0x09, 0x0305, 1, data, sizeof(data), 1000);
        if(rc < 0) {
            return -1;
        }
    }

    if (1) { // receive command
        unsigned char data[6] = {};

        int rc = libusb_control_transfer(devh, 0xa1, 0x01, 0x0305, 1, data, sizeof(data), 2000);
        if(rc < 0) {
            return -1;
        }

        if (data[0] != SHORTREPOID || data[1] != pass) {
            printf(">>> Verify mismatch: type=%02x, pass=%02x, received=%02x\n", type, pass, data[1]);
            return -1;
        }
    }

    return 0;
}

int try_touchpad_verify(int type, int pass, int sendcmd)
{
    int try;

    for (try = 0; try < 100; try++) {
        usleep(50*1000);

        int rc = touchpad_verify(type, pass, sendcmd);
        if (rc == 0) {
            break;
        }
    }

    if (try == 100) {
        printf("EEE Touchpad verify data failed type:%d pass:%d\n", type, pass);
        return -1;
    }

    return 0;
}

int write_tp_fw(const unsigned char *fw, int fw_length)
{
    int block_size = 1024;
    int try;
    int rc;

    if (fw_length < 24576) {
        printf("EEE Bad touchpad firmware size expected:%d received:%d\n",
            fw_length, 24576);
        return -1;
    }

    // cap to 24k
    fw_length = 24 * 1024;

    printf("[*] Opening USB device in touchpad mode...\n");
    for (try = 0; try < 3; try++) {
        rc = open_touchpad_mode();
        if (rc >= 0) {
            break;
        }
        usleep(500*1000);
    }

    if (try >= 3) {
        printf("EEE Failed to open in touchpad mode\n");
        goto finish;
    }

    rc = try_touchpad_verify(ICERASE, ICERASE_PASS, 0);
    if (rc < 0) {
        printf("EEE Touchpad erase failed\n");
        goto finish;
    }

    usleep(10000);

    for(int offset = 0; offset < fw_length; offset += block_size)
    {
        unsigned char data[16 + block_size];
        unsigned char *ptr = data;

        *ptr++ = 0x06;
        *ptr++ = 0xD0;
        *ptr++ = 0xCC;
        *ptr++ = 0xCC;
        *ptr++ = offset & 0xFF;
        *ptr++ = (offset >> 8) & 0xFF;
        *ptr++ = 0xCC;
        *ptr++ = 0xCC;

        memset(ptr, 0, block_size);
        memcpy(ptr, &fw[offset], MIN(block_size, fw_length-offset));
        ptr += block_size;
        
        *ptr++ = 0xEE;
        *ptr++ = 0xD2;
        *ptr++ = 0xCC;
        *ptr++ = 0xCC;
        *ptr++ = 0xCC;
        *ptr++ = 0xCC;
        *ptr++ = 0xCC;
        *ptr++ = 0xCC;

        printf(">>> USB write offset:%d length:%d...\n", offset, block_size);

        rc = libusb_control_transfer(devh, 0x21, 0x09, 0x0306, 1, data, sizeof(data), 1000);
        if (rc < 0) {
            printf("EEE USB write failed\n");
            goto finish;
        }
        
        usleep(150*1000);

        printf(">>> Verifying touchpad '1k-data'...\n");

        rc = try_touchpad_verify(VERIFY1KDATA, VERIFY1KDATA_PASS, 1);
        if (rc < 0) {
            printf("EEE Touchpad verify '1k-data' failed\n");
            goto finish;
        }
    }

    usleep(50*1000);
    
    printf("[*] Verifying touchpad 'end-program'...\n");

    rc = try_touchpad_verify(ENDPROGRAM, ENDPROGRAM_PASS, 1);
    if (rc < 0) {
        printf("EEE Touchpad verify 'end-program' failed\n");
        goto finish;
    }

    usleep(50*1000);

    printf("[*] Verifying touchpad 'checksum'...\n");

    rc = try_touchpad_verify(VERIFY_CHECKSUM, VERIFY_CHECKSUM_PASS, 1);
    if (rc < 0) {
        printf("EEE Touchpad verify 'checksum' failed\n");
        goto finish;
    }

    usleep(50*1000);

    printf("[*] Verifying touchpad 'program'...\n");
    
    rc = try_touchpad_verify(PROGRAMPASS, 0, 1);
    if (rc < 0) {
        printf("EEE Touchpad verify 'program' failed\n");
        goto finish;
    }

    printf("[*] Touchpad update completed successfully\n");

finish:
    close_usb();
    return rc;
}
