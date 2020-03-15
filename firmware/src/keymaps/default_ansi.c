/* Copyright 2020 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "keymap.h"
#include "sh68f83.h"

// there's a big chunk of space that's empty, so we're gonna put our fn arrays
// there instead of the usual spot, so we can enable any key to have a fn 
#define STARTING_ADDR 0x3000

__code __at (0x0815) uint16_t keymap[] = KEYMAP_ANSI(
    FR(11), FS(0), FS(1), R(F3), FS(2), FS(3), FS(4), FS(5), FR(1), FR(2), FR(3), FR(4), FR(5),
    R(GRV),  R(1), R(2), R(3), R(4), R(5), R(6), FK(7), FK(8), FK(9), FK(10), R(MINS), R(EQL), FR(10) , 
    R(TAB),   R(Q), R(W), R(E), R(R), R(T), R(Y), FK(4), FK(5), FK(6), FK(11), R(LBRC), R(RBRC), R(BSLS),
    R(CAPS),   R(A), R(S), R(D), R(F), R(G), R(H), FK(1), FK(2), FK(3), FK(12), R(QUOT), R(ENT),
    M(LSFT),    FR(0), R(X), R(C), R(V), R(B), R(N), FK(0), R(COMM), FK(13), FK(14), M(RSFT), FR(6), 
    M(LCTL), F(0), M(LGUI), M(LALT),         R(SPC),           M(RALT), M(RCTL),        FR(7), FR(8), FR(9)
);

// __code __at (0x0935)
__code uint16_t fns_special[] = {
    [0] = REG_FN(KC_F1, 0xA5), // reversed keycodes mapped brightness up in OS
    [1] = REG_FN(KC_F2, 0xA6), // reversed keycodes mapped brightness dn in OS
    [2] = REG_FN(KC_F4, KC__MUTE),
    [3] = REG_FN(KC_F5, KC__VOLDOWN),
    [4] = REG_FN(KC_F6, KC__VOLUP),
    [5] = REG_FN(KC_F7, 0xFE) // toggle touchpad
};

// __code __at (0x0941)
__code uint16_t fns_regular[] = {
    [0] = REG_FN(KC_Z, KC_NUBS),
    [1] = REG_FN(KC_F8, KC_INS),
    [2] = REG_FN(KC_F9, KC_PSCR),
    [3] = REG_FN(KC_F10, KC_NLCK),
    [4] = REG_FN(KC_F11, KC_SLCK),
    [5] = REG_FN(KC_F12, KC_PAUS),
    [6] = REG_FN(KC_UP, KC_PGUP),
    [7] = REG_FN(KC_LEFT, KC_HOME),
    [8] = REG_FN(KC_DOWN, KC_PGDN),
    [9] = REG_FN(KC_RGHT, KC_END),
    [10] = REG_FN(KC_BSPC, KC_DEL),
    [11] = REG_FN(KC_ESC, KC_POWER)
};

// __code __at (0x0957)
__code uint16_t fns_keypad[] = {
    [0] = REG_FN(KC_M, KC_P0),
    [1] = REG_FN(KC_J, KC_P1),
    [2] = REG_FN(KC_K, KC_P2),
    [3] = REG_FN(KC_L, KC_P3),
    [4] = REG_FN(KC_U, KC_P4),
    [5] = REG_FN(KC_I, KC_P5),
    [6] = REG_FN(KC_O, KC_P6),
    [7] = REG_FN(KC_7, KC_P7),
    [8] = REG_FN(KC_8, KC_P8),
    [9] = REG_FN(KC_9, KC_P9),
    [10] = REG_FN(KC_0, KC_PAST),
    [11] = REG_FN(KC_P, KC_PMNS),
    [12] = REG_FN(KC_SCLN, KC_PPLS),
    [13] = REG_FN(KC_DOT, KC_PDOT),
    [14] = REG_FN(KC_SLSH, KC_PSLS)
};

// custom serial string "ANSI 0.1" in place of old "sino wealth" one
ADDR(0x007E) = 0x14; // lenth of packet
ADDR(0x007F) = 0x03; // string
ADDR(0x0080)[] = {
    0x41, 0x00, 0x4E, 0x00, 0x53, 0x00, 0x49, 0x00, 0x20, 0x00, 0x30, 0x00,
    0x2E, 0x00, 0x31, 0x00, 0x00, 0x00 // null term
};

#define GET_DPL(addr) ((addr) & 0xFF)
#define GET_DPH(addr) (((addr) >> 8) & 0xFF)

#define FN_SPECIAL_ADDR STARTING_ADDR
#define FN_REGULAR_ADDR FN_SPECIAL_ADDR + sizeof(fns_special)
#define FN_KEYPAD_ADDR FN_REGULAR_ADDR + sizeof(fns_regular)

// assign our custom array addresses to the DPTR loading spots

__code __at (0x18E3) uint8_t fns_special_reg_dpl = GET_DPL(FN_SPECIAL_ADDR);
__code __at (0x18E8) uint8_t fns_special_reg_dph = GET_DPH(FN_SPECIAL_ADDR);

__code __at (0x1909) uint8_t fns_special_fn_dpl = GET_DPL(FN_SPECIAL_ADDR + 1);
__code __at (0x190E) uint8_t fns_special_fn_dph = GET_DPH(FN_SPECIAL_ADDR + 1);

__code __at (0x18C0) uint8_t fns_regular_reg_dpl = GET_DPL(FN_REGULAR_ADDR);
__code __at (0x18C5) uint8_t fns_regular_reg_dph = GET_DPH(FN_REGULAR_ADDR);

__code __at (0x1915) uint8_t fns_regular_fn_dpl = GET_DPL(FN_REGULAR_ADDR + 1);
__code __at (0x191A) uint8_t fns_regular_fn_dph = GET_DPH(FN_REGULAR_ADDR + 1);

__code __at (0x06D5) uint8_t fns_keypad_reg_dpl1 = GET_DPL(FN_KEYPAD_ADDR);
__code __at (0x06DA) uint8_t fns_keypad_reg_dph1 = GET_DPH(FN_KEYPAD_ADDR);
__code __at (0x079D) uint8_t fns_keypad_reg_dpl2 = GET_DPL(FN_KEYPAD_ADDR);
__code __at (0x07A2) uint8_t fns_keypad_reg_dph2 = GET_DPH(FN_KEYPAD_ADDR);

__code __at (0x18F4) uint8_t fns_keypad_fn_dpl = GET_DPL(FN_KEYPAD_ADDR + 1);
__code __at (0x18F9) uint8_t fns_keypad_fn_dph = GET_DPH(FN_KEYPAD_ADDR + 1);
