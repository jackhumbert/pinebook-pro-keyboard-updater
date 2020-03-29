#include "revised.h"

// make the end of L0456 jump to this and use the return to pop back
ADDR(0x07FE)[] = { 0x02, 0x26, 0x9F }; // LJMP L0138
// instruction above overwrites L0406 so we need to change it a bit
ADDR(0x0801)[] = { 0xA2, 0x0A,      // MOV C, 0Ah
                   0x33,            // RLC A (already 0)
                   0xFF,            // MOV R7, A
                   0x7D, 0x08,      // MOV R5, #8h
                   0x02, 0x26, 0xC3 // LJMP L0370, use ret to pop back
                 };
// fix the calls to the function above
ADDR(0x070A)[] = { 0x12, 0x08, 0x01 };
ADDR(0x07DD)[] = { 0x12, 0x08, 0x01 };

// f10 process to new function
ADDR(0x0620)[] = { 0x02, 0x06, 0x93 }; // LJMP NEW

// f11 process
ADDR(0x0658)[] = { 0x02, 0x06, 0x93 }; // LJMP NEW

// f12 process
ADDR(0x0690)[] = { 0x12, 0x07, 0xE4 }; // LCALL L0456

// process R5=keycode and R7=pressed
// make new function like L0369, then jump to L0374
// add 4 if we're turning on the feature
// put in the extra space around L0466
// 06AC is the last address we can use here

// NEW:
ADDR(0x0693)[] = {
    0xE5, 0x64,       // MOV A, 64h  ; grabs the index of the f key
    0x24, 0x65,       // ADD A, #65h ; bumps these actions to F13+
    0xFD,             // MOV R5, A
    0xE5, 0x12,       // MOV A, 12h  ; figure out if we're enabling
    0x54, 0x02,       // ANL A, #2h
    0x33,             // RLC A
    0x2D,             // ADD A, R5
    0xFD,             // MOV R5, A
    0x7F, 0x01,       // MOV R7, #1h ; pressing the key
    0x12, 0x1B, 0xFF, // LCALL L0374
    0x12, 0x17, 0x0D, // LCALL L0143 ; send action
    0x7F, 0x00,       // MOV R7, #0h ; releasing the key
    0x02, 0x1B, 0xFF  // LJMP L0374  ; process as usual
};

// old non-privacy toggle jump to regular process (arrow key fix)
ADDR(0x066A)[] = { 0x02, 0x06, 0xAD }; // LJMP L0449

// ignore the numlock feature, only enable in fn layer
ADDR(0x06CF)[] = { 0x02, 0x06, 0xD4 }; // LJMP L0436

// add another fn layer

// in L0361
// 056E jump to custom
// if r0A 
    // r?? |= r64
// else
    // r?? &= ~r64
// jump to 0577

// b26 mentions
// 0571 // skipping
// 0575 // skipping
// 05AC // in 0x06 press
// 05D5 // in 0x07 press
// 06BA // in 0x0A press
// 06E9 // in 0x09 press
// 06FC // in 0x0C press

// open vars
// 2C


// r65 values to switch/case
//   0x01: -
//   0x02: L0370
//   0x03: L0384
//   0x04: 
//   0x05: L0389
//   0x06: L0394
//   0x07: L0398
//   0x0A: L0395
//   0x09: L0400
//   0x0C: L0403

// custom manufacturer
ADDR(0x009B) = 0x1E; // length of packet
ADDR(0x009C) = 0x03; // string
ADDR(0x009D)[] = { // Pine64
    0x50, 0x00, 0x69, 0x00, 0x6e, 0x00, 0x65, 0x00, 0x36, 0x00, 0x34, 0x00,
    0x00, 0x00
};

// custom product
ADDR(0x00BC) = 0x1A; // length of packet
ADDR(0x00BD) = 0x03; // string
ADDR(0x00BE)[] = { // Pinebook Pro
    0x50, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x65, 0x00, 0x62, 0x00, 0x6F, 0x00,
    0x6F, 0x00, 0x6B, 0x00, 0x20, 0x00, 0x50, 0x00, 0x72, 0x00, 0x6F, 0x00,
    0x00, 0x00
};

// add product to configuration and interface string
ADDR(0x043E) = 0xFF; // DB 0FFh ; iConfiguration (ANSI 0.1)
ADDR(0x0449) = 0x02; // DB 002h ; iInterface (Pinebook Pro)

// tell serial to look at 0xFF
ADDR(0x010D) = 0xFF; // DB 0FFh ; serial number

// custom bcdDevice version (0.1)
ADDR(0x0109)[] = { 0x01, 0x00 }; // DB 00100h ; bcd device version

// Fix the touchpad HID Usage Page
ADDR(0x01d0)[] = { 0xB3, 0x03 }; // Usage (X) Physical Maximum 947 x 10^-2 cm
ADDR(0x01da)[] = { 0x6F, 0x02 }; // Usage (Y) Physical Maximum 623 x 10^-2 cm
