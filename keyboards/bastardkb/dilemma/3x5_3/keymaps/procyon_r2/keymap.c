/**
 * Copyright 2022 Charly Delay <charly@codesink.dev> (@0xcharly)
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

#include QMK_KEYBOARD_H
#include "digitizer.h"

enum dilemma_keymap_layers {
    LAYER_BASE = 0,
    LAYER_NAV,
    LAYER_SYM,
    LAYER_NUM,
};

#define NAV QK_TRI_LAYER_LOWER
#define SYM QK_TRI_LAYER_UPPER

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT_split_3x5_3(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
          KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,       KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
          KC_A,    KC_S,    KC_D,    KC_F,    KC_G,       KC_H,    KC_J,    KC_K,    KC_L, KC_QUOT,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,       KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         KC_LCTL,     NAV, KC_LGUI,     KC_SPC,     SYM,    KC_A
  //                   ╰───────────────────────────╯ ╰──────────────────────────╯
  ),

  [LAYER_NAV] = LAYOUT_split_3x5_3(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
        KC_TAB, XXXXXXX, XXXXXXX, XXXXXXX, KC_VOLU,    XXXXXXX, KC_HOME,   KC_UP,  KC_END,  KC_DEL,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI, KC_VOLD,    XXXXXXX, KC_LEFT, KC_DOWN, KC_RGHT, KC_BSPC,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       QK_BOOT, EE_CLR, KC_MPRV, KC_MNXT, KC_MPLY,    XXXXXXX, KC_PGDN, KC_PGUP, XXXXXXX,  KC_ENT,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         XXXXXXX, _______, KC_LSFT,     KC_SPC, _______, KC_ESC
  //                   ╰───────────────────────────╯ ╰──────────────────────────╯
  ),

  [LAYER_SYM] = LAYOUT_split_3x5_3(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
        KC_ESC, KC_LBRC, KC_LCBR, KC_LPRN, KC_TILD,    KC_CIRC, KC_RPRN, KC_RCBR, KC_RBRC,  KC_GRV,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       KC_MINS, KC_ASTR,  KC_EQL, KC_UNDS,  KC_DLR,    KC_HASH, KC_RGUI, KC_RALT, KC_RCTL, KC_RSFT,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       KC_PLUS, KC_PIPE,   KC_AT, KC_SLSH, KC_PERC,    KC_SCLN, KC_BSLS, KC_AMPR, KC_QUES, KC_EXLM,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                          KC_ESC, _______, KC_LSFT,     KC_SPC, _______, XXXXXXX
  //                   ╰───────────────────────────╯ ╰──────────────────────────╯
  ),

  [LAYER_NUM] = LAYOUT_split_3x5_3(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
          KC_1,    KC_2,    KC_3,    KC_4,    KC_5,       KC_6,    KC_7,    KC_8,    KC_9,   KC_0,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI,  KC_F11,     KC_F12, KC_RGUI, KC_RALT, KC_RCTL, KC_RSFT,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
         KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,      KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         XXXXXXX, _______, KC_LSFT,     KC_SPC, _______, XXXXXXX
  //                   ╰───────────────────────────╯ ╰──────────────────────────╯
  ),
};
// clang-format on

#define INVALID_ID 0xff

// If defined, removing all fingers from the trackpad will not cancel the held button. To cancel repeat the drag gesture.
#define HOLD_LOCK

typedef enum {
    None,
    PossibleMove,
    Move,
    PossibleHold,
    Hold,
    Abort
} State;

bool digitizer_task_kb(digitizer_t *const digitizer_state) {
    uint8_t contact_count = 0;
    static uint8_t first_contact_id = INVALID_ID;
    static uint32_t timer = 0;
    static State state = None;

    // First count up the number of fingers, and if we found a first touch
    // keep a record of the finger id.
    for (int i = 0; i < DIGITIZER_FINGER_COUNT; i++) {
        if(digitizer_state->contacts[i].tip) {
            contact_count++;
            if (first_contact_id == INVALID_ID) {
                first_contact_id = i;
                state = PossibleMove;
                timer = timer_read32();
            }
        }
        else {
            // If the initial contact is removed, abort.
#ifdef HOLD_LOCK
            if (state != Hold)
#endif
            if (i == first_contact_id) {
                first_contact_id = INVALID_ID;
                state = Abort;
            }
        }
    }

    // If all contacts are removed, reset and wait for new touches.
#ifdef HOLD_LOCK
    if (state != Hold)
#endif
    if (contact_count == 0) {
        state = None;
        first_contact_id = INVALID_ID;
    }

    switch (state) {
        case None:
            break;
        case PossibleMove:
            // If we got a single touch, we wait for 100ms before activating the drag detection.
            // This should prevent two finger taps as being detected as hold.

            // If we detect anything other than 1 finger, abort.
            if (contact_count != 1) {
                state = Abort;
            }
            else if (timer_elapsed(timer) > 100) {
                state = Move;
            }
            break;
        case Move:
            // We are now detecting drag gestures. If we detect a second finger, its a possible hold.
            if (contact_count == 2) {
                state = PossibleHold;
                timer = timer_read32();
            }
            break;
        case PossibleHold:
            // If the second finger is released within 250ms, its a tap. So activate the hold gesture.
            if (contact_count == 1) {
                state = Hold;
                digitizer_state->button1 = true;
            }
            else if (timer_elapsed(timer) > 250) {
                state = Abort;
            }
            break;
        case Hold:
        {
            // If the user taps with a second finger again, release the hold.
            static uint8_t last_contact_count = 0;
            if (contact_count == 1 && last_contact_count > 1) {
                state = Move;
            }
            last_contact_count = contact_count;
            // While we are holding, clear all touches other than the first finger
            digitizer_state->button1 = true;
            break;
        }
        case Abort:
            break;
    }
    return state == Hold;
}