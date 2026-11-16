/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
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

enum charybdis_keymap_layers {
    LAYER_BASE = 0,
    LAYER_QWERTY,
    LAYER_EXTEND,
    LAYER_SYMBOLS,
    LAYER_NUMBERS,
    LAYER_FUNCTIONS,
    LAYER_MOUSE,
};

enum custom_keycodes {
    DRAG_SCROLL = SAFE_RANGE,
    MS_ESC,
    INCREASE_CPI,
    DEFAULT_CPI,
    DECREASE_CPI,
    TOGGLE_RGB,
};

#define MS_SCRL DRAG_SCROLL
#define MS_ICPI INCREASE_CPI
#define MS_DCPI DECREASE_CPI
#define MS_DFLT DEFAULT_CPI

#define OS_LCTL OSM(MOD_LCTL)
#define OS_LALT OSM(MOD_LALT)
#define OS_LGUI OSM(MOD_LGUI)
#define OS_LSFT OSM(MOD_LSFT)

#define MT_ACTL MT(MOD_LCTL, KC_A)
#define MT_RALT MT(MOD_LALT, KC_R)
#define MT_SGUI MT(MOD_LGUI, KC_S)
#define MT_TSFT MT(MOD_LSFT, KC_T)
#define MT_NSFT MT(MOD_LSFT, KC_N)
#define MT_EGUI MT(MOD_LGUI, KC_E)
#define MT_IALT MT(MOD_LALT, KC_I)
#define MT_OCTL MT(MOD_LCTL, KC_O)

#define LG_PLUS LGUI(KC_PLUS)
#define LG_MINS LGUI(KC_MINS)
#define LG_0 LGUI(KC_0)

#define OP_CUT LGUI(KC_X)
#define OP_COPY LGUI(KC_C)
#define OP_PSTE LGUI(KC_V)
#define OP_UNDO LGUI(KC_Z)
#define OP_CPHS LSFT(LGUI(KC_C))
#define OP_PWOF LSFT(LGUI(KC_V))

#define BR_TLFT LGUI(KC_LBRC)
#define BR_TRGT LGUI(KC_RBRC)

#define NUM_ESC LT(LAYER_NUMBERS, KC_ESC)
#define L_EXT MO(LAYER_EXTEND)
#define L_SYM MO(LAYER_SYMBOLS)

#define V_MUTE KC_KB_MUTE
#define V_UP KC_KB_VOLUME_UP
#define V_DOWN KC_KB_VOLUME_DOWN
#define RGB_TGL TOGGLE_RGB

#define DEFAULT_MOUSE_CPI 1000

#define SCROLL_DIVISOR_H 8.0
#define SCROLL_DIVISOR_V 8.0

float    scroll_accumulated_h = 0;
float    scroll_accumulated_v = 0;
uint16_t current_mouse_cpi    = DEFAULT_MOUSE_CPI;

bool set_scrolling = false;

layer_state_t layer_state_set_user(layer_state_t state) {
    if (get_highest_layer(state) != LAYER_MOUSE) {
        set_scrolling = false;
        pointing_device_set_cpi(current_mouse_cpi);
    }
    return update_tri_layer_state(state, LAYER_SYMBOLS, LAYER_EXTEND, LAYER_FUNCTIONS);
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // Check if drag scrolling is active
    if (set_scrolling) {
        // Calculate and accumulate scroll values based on mouse movement and divisors
        scroll_accumulated_h += (float)mouse_report.x / SCROLL_DIVISOR_H;
        scroll_accumulated_v += (float)mouse_report.y / SCROLL_DIVISOR_V;

        // Assign integer parts of accumulated scroll values to the mouse report
        mouse_report.h = -(int8_t)scroll_accumulated_h;
        mouse_report.v = -(int8_t)scroll_accumulated_v;

        // Update accumulated scroll values by subtracting the integer parts
        scroll_accumulated_h -= (int8_t)scroll_accumulated_h;
        scroll_accumulated_v -= (int8_t)scroll_accumulated_v;

        // Clear the X and Y values of the mouse report
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint16_t new_cpi = 0;
    switch (keycode) {
        case DRAG_SCROLL:
            // Toggle set_scrolling when DRAG_SCROLL key is pressed or released
            if (record->event.pressed) {
                current_mouse_cpi = pointing_device_get_cpi();
                pointing_device_set_cpi(PMW33XX_CPI_MIN);
            } else {
                pointing_device_set_cpi(current_mouse_cpi);
            }
            set_scrolling = record->event.pressed;
            break;
        case MS_ESC:
            if (record->event.pressed) {
                auto_mouse_layer_off();
            }
            break;
        case TOGGLE_RGB:
            if (record->event.pressed) {
                rgb_matrix_toggle();
            }
            break;
        case INCREASE_CPI:
            if (record->event.pressed) {
                new_cpi = pointing_device_get_cpi() + PMW33XX_CPI_STEP;
                if (new_cpi > PMW33XX_CPI_MAX) {
                    new_cpi = PMW33XX_CPI_MAX;
                }
                current_mouse_cpi = new_cpi;
                pointing_device_set_cpi(new_cpi);
            }
            break;
        case DECREASE_CPI:
            if (record->event.pressed) {
                new_cpi = pointing_device_get_cpi() - PMW33XX_CPI_STEP;
                if (new_cpi < PMW33XX_CPI_MIN) {
                    new_cpi = PMW33XX_CPI_MIN;
                }
                current_mouse_cpi = new_cpi;
                pointing_device_set_cpi(new_cpi);
            }
            break;
        case DEFAULT_CPI:
            if (record->event.pressed) {
                current_mouse_cpi = DEFAULT_MOUSE_CPI;
                pointing_device_set_cpi(DEFAULT_MOUSE_CPI);
            }
            break;
        default:
            break;
    }
    return true;
}

bool is_mouse_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DRAG_SCROLL ... DECREASE_CPI:
            return true;
        default:
            return false;
    }
}

void pointing_device_init_user(void) {
    set_auto_mouse_layer(LAYER_MOUSE);
    set_auto_mouse_enable(true);
    set_auto_mouse_debounce(50);
    pointing_device_set_cpi(DEFAULT_MOUSE_CPI);
}

const key_override_t left_key_h_override  = ko_make_basic(MOD_MASK_CTRL, KC_LEFT, LCTL(KC_H));
const key_override_t down_key_j_override  = ko_make_basic(MOD_MASK_CTRL, KC_DOWN, LCTL(KC_J));
const key_override_t up_key_k_override    = ko_make_basic(MOD_MASK_CTRL, KC_UP, LCTL(KC_K));
const key_override_t right_key_l_override = ko_make_basic(MOD_MASK_CTRL, KC_RGHT, LCTL(KC_L));

const key_override_t *key_overrides[] = {
    // clang-format off
    &left_key_h_override,
    &down_key_j_override,
    &up_key_k_override,
    &right_key_l_override,
    // clang-format on
};

// clang-format off
// Handedness for Chordal Hold: the left half is matrix rows 0-3, the right half rows 4-7.
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = LAYOUT(
    'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R',
    'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R',
    'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R',
              'L', 'L', 'L',  'R', 'R'
);

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
          KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,       KC_J,    KC_L,    KC_U,    KC_Y, KC_QUOT,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       MT_ACTL, MT_RALT, MT_SGUI, MT_TSFT,    KC_G,       KC_M, MT_NSFT, MT_EGUI, MT_IALT, MT_OCTL,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
          KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,       KC_K,    KC_H, KC_COMM,  KC_DOT, KC_SLSH,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         NUM_ESC,   L_EXT, KC_LSFT,     KC_SPC,   L_SYM
  //                   ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_QWERTY] = LAYOUT(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
          KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,       KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
          KC_A,    KC_S,    KC_D,    KC_F,    KC_G,       KC_H,    KC_J,    KC_K,    KC_L,    KC_J,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,       KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         NUM_ESC,   L_EXT, KC_LSFT,     KC_SPC,   L_SYM
  //                   ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_EXTEND] = LAYOUT(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
        KC_ESC, BR_TLFT, XXXXXXX, BR_TRGT,  KC_INS,    KC_HOME, KC_PGDN, KC_PGUP,  KC_END, XXXXXXX,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       OS_LCTL, OS_LALT, OS_LGUI, OS_LSFT, KC_RALT,    KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, KC_PMNS,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       OP_UNDO,  OP_CUT, OP_COPY, KC_LGUI, OP_PSTE,     KC_ENT, KC_BSPC,  KC_TAB, XXXXXXX,  KC_ENT,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         _______, _______, _______,     KC_ENT, _______
  //                   ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_SYMBOLS] = LAYOUT(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮j
       KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,     KC_EQL,  KC_GRV, KC_COLN, KC_SCLN, KC_PLUS,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       OS_LCTL, OS_LALT, OS_LGUI, OS_LSFT, KC_CIRC,    KC_ASTR, KC_LPRN, KC_LCBR, KC_LBRC, KC_MINS,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       KC_MINS, KC_UNDS, KC_BSLS, KC_PIPE, KC_AMPR,    KC_TILD, KC_RPRN, KC_RCBR, KC_RBRC, KC_UNDS,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         _______, _______, _______,    _______, _______
  //                   ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_NUMBERS] = LAYOUT(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
       RGB_TGL, _______, _______, _______, _______,     KC_EQL,    KC_7,    KC_8,    KC_9, KC_PLUS,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       OS_LCTL, OS_LALT, OS_LGUI, OS_LSFT, KC_CAPS,    KC_ASTR,    KC_4,    KC_5,    KC_6, KC_MINS,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       _______, _______,  KC_TAB, KC_BSPC,  KC_ENT,       KC_0,    KC_1,    KC_2,    KC_3, KC_UNDS,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         _______, _______, _______,    _______, _______
  //                   ╰───────────────────────────╯ ╰──────────────────╯
  ),
  [LAYER_FUNCTIONS] = LAYOUT(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
       _______, _______, _______, _______, KC_BRIU,     KC_F12,   KC_F7,   KC_F8,   KC_F9, LG_PLUS,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       OS_LCTL, OS_LALT, OS_LGUI, OS_LSFT, KC_BRID,     KC_F11,   KC_F4,   KC_F5,   KC_F6, LG_MINS,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
        V_MUTE,  V_DOWN, OP_CPHS,    V_UP, OP_PWOF,     KC_F10,   KC_F1,   KC_F2,   KC_F3,    LG_0,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         _______, _______, _______,    _______, _______
  //                   ╰───────────────────────────╯ ╰──────────────────╯
  ),
  [LAYER_MOUSE] = LAYOUT(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
        MS_ESC, _______, _______, _______, _______,    _______, MS_ICPI, MS_DCPI, MS_DFLT, _______,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       KC_LCTL, KC_LALT, KC_LGUI, KC_LSFT, _______,    _______, MS_BTN1, MS_BTN2, _______, _______,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       _______, _______, _______, _______, _______,    _______, MS_BTN1, MS_BTN2, MS_BTN3, _______,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         _______, MS_SCRL, _______,    _______, _______
  //                   ╰───────────────────────────╯ ╰──────────────────╯
  ),
};
// clang-format on
