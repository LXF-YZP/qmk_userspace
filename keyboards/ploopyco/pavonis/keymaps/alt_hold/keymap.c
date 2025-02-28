// Copyright 2024 George Norton (@george-norton)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include <math.h>
#include "digitizer.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {{{ KC_NO }}};

#define INVALID_ID 0xff

typedef enum {
    None,
    PossibleMove,
    Move,
    PossibleHold,
    Hold,
    Abort
} State;

digitizer_t digitizer_task_kb(digitizer_t digitizer_state) {
    uint8_t contact_count = 0;
    static uint8_t first_contact_id = INVALID_ID;
    static uint32_t timer = 0;
    static State state = None;

    // First count up the number of fingers, and if we found a first touch
    // keep a record of the finger id.
    for (int i = 0; i < DIGITIZER_FINGER_COUNT; i++) {
        if(digitizer_state.contacts[i].tip) {
            contact_count++;
            if (first_contact_id == INVALID_ID) {
                first_contact_id = i;
                state = PossibleMove;
                timer = timer_read32();
            }
        }
        else {
            // If the initial contact is removed, abort.
            if (i == first_contact_id) {
                first_contact_id = INVALID_ID;
                state = Abort;
            }
        }
    }

    // If all contacts are removed, reset and wait for new touches.
    if (contact_count == 0) {
        state = None;;
        first_contact_id = INVALID_ID;
    }

    switch (state) {
        case None:
            break;
        case PossibleMove:
            // If we got a single touch, we wait for 250ms before activating the drag detection.
            // This should prevent two finger taps as being detected as hold.

            // If we detect anything other than 1 finger, abort. 
            if (contact_count != 1) {
                state = Abort;
            }
            else if (timer_elapsed(timer) > 250) {
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
                digitizer_state.button1 = true;
            }
            else if (timer_elapsed(timer) > 250) {
                state = Abort;
            }
            break;
        case Hold:
        {
            // While we are holding, clear all touches other than the initial finger
            for (int i = 0; i < DIGITIZER_FINGER_COUNT; i++) {
                if (i != first_contact_id) {
                    digitizer_state.contacts[i].tip = false;
                }
            }
            digitizer_state.button1 = true;
            break;
        }
        case Abort:
            break;
    }

    return digitizer_state;
}
