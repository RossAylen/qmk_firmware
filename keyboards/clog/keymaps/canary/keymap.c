#include QMK_KEYBOARD_H

#include "features/achordion.h"
#include "features/custom_shift_keys.h"

///////////////////////////////////////////////////////////////////////////////
// Aliases and definitions
///////////////////////////////////////////////////////////////////////////////

enum layers {
    BASE = 0,
    SYM,
    NAV,
    NUM,
};

enum custom_keycodes {
    SELLINE = SAFE_RANGE, // Select the current line
    STD_CLN,              // std::
    USRNAME,              // Ross
    UP_DIR,               // ../
    // Custom repeat key
    M_UPDIR,   // . -> ../
    M_TDIR,    // , -> ./
    M_INCLUDE, // # -> #include
    M_DOCSTR,  // " -> """<cursor>"""
    M_MKGRVS,  // ` -> ```<cursor>```
};

/**
 *  w l y p b   z f o u '
 *  c r s t g   m n e i a
 *  q j v d k   x h ; , .
 */

// Left hand home row
#define HOME_C LT(SYM, KC_C)
#define HOME_R LALT_T(KC_R)
#define HOME_S LSFT_T(KC_S)
#define HOME_T LCTL_T(KC_T)

// Right hand home row
#define HOME_N RCTL_T(KC_N)
#define HOME_E RSFT_T(KC_E)
#define HOME_I LALT_T(KC_I) // I3 only reads left alt
#define HOME_A LT(SYM, KC_A)

// Pinky's lower gui's
#define PNKY_Q LGUI_T(KC_Q)
#define PNKY_DOT LGUI_T(KC_DOT)

// Thumb keys (left/right and inner/outer)
#define THMB_LI KC_SPC
#define THMB_RI QK_AREP

///////////////////////////////////////////////////////////////////////////////
// My Keymap
///////////////////////////////////////////////////////////////////////////////

// clang-format off

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [BASE] = LAYOUT_split_3x5_2(
        KC_W,     KC_L,    KC_Y,    KC_P,    KC_B,        KC_Z,    KC_F,    KC_O,    KC_U,  KC_QUOT,
        HOME_C, HOME_R,  HOME_S,  HOME_T,    KC_G,        KC_M,  HOME_N,  HOME_E,  HOME_I,   HOME_A,
        PNKY_Q,   KC_J,    KC_V,    KC_D,    KC_K,        KC_X,    KC_H, KC_SCLN, KC_COMM, PNKY_DOT,

                                 MO(NAV), THMB_LI,        THMB_RI, MO(NUM)
    ),

    [SYM] = LAYOUT_split_3x5_2(
        XXXXXXX, KC_LABK, KC_RABK, KC_BSLS, USRNAME,      KC_PIPE, STD_CLN, KC_LBRC, KC_RBRC, XXXXXXX,
        KC_EXLM, KC_MINS, KC_PLUS,  KC_EQL, KC_HASH,      KC_AMPR, KC_COLN, KC_LPRN, KC_RPRN, KC_PERC,
        XXXXXXX, KC_SLSH, KC_ASTR, KC_CIRC, KC_GRV,       KC_TILD,  KC_DLR, KC_LCBR, KC_RCBR, XXXXXXX,

                                   _______, KC_UNDS,      _______, _______
    ),

    //  NAV layers doubles as mousing layer (i.e. left hand shortcuts)

    [NAV] = LAYOUT_split_3x5_2(
        C(KC_Z),  KC_DEL, C(S(KC_V)), XXXXXXX,    XXXXXXX,       XXXXXXX, XXXXXXX, SELLINE, XXXXXXX, XXXXXXX,
        C(KC_C), KC_LALT,    KC_LSFT, KC_LCTL, C(KC_SLSH),       KC_PGUP, KC_LEFT,   KC_UP, KC_RGHT, XXXXXXX,
        KC_LGUI, C(KC_X),    C(KC_V), C(KC_D),    XXXXXXX,       KC_PGDN, KC_HOME, KC_DOWN,  KC_END, XXXXXXX,

                                        _______,  _______,       _______, _______
    ),

    [NUM] = LAYOUT_split_3x5_2(
        XXXXXXX,  KC_4,    KC_5,    KC_6,    XXXXXXX,       XXXXXXX, C(KC_MINS), XXXXXXX, C(KC_EQL), XXXXXXX,
           KC_0,  KC_1,    KC_2,    KC_3,     KC_DOT,       KC_VOLU,    KC_RCTL, KC_RSFT,   KC_LALT, XXXXXXX,
        XXXXXXX,  KC_7,    KC_8,    KC_9,    XXXXXXX,       KC_VOLD,    XXXXXXX, XXXXXXX,   XXXXXXX, KC_LGUI,

                                   XXXXXXX,  _______,       _______, _______
    ),
};

// clang-format on

///////////////////////////////////////////////////////////////////////////////
// Custom shift keys (https://getreuer.info/posts/keyboards/custom-shift-keys)
///////////////////////////////////////////////////////////////////////////////

/**
 *  w l y p b   z f o u '
 *  c r s t g   m n e i a
 *  q j v d k   x h ; , .
 */

custom_shift_key_t const custom_shift_keys[] = {
    {KC_SCLN, KC_AT},   // ; -> @
    {KC_COMM, KC_QUES}, // . -> ?
    {KC_EQL, KC_EQL},   // Don't shift =
    {KC_SLSH, KC_SLSH}, // Don't shift /
};

uint8_t NUM_CUSTOM_SHIFT_KEYS = sizeof(custom_shift_keys) / sizeof(custom_shift_key_t);

///////////////////////////////////////////////////////////////////////////////
// Combos (https://docs.qmk.fm/#/feature_combo)
///////////////////////////////////////////////////////////////////////////////

/**
 *  w l y p b   z f o u '
 *  c r s t g   m n e i a
 *  q j v d k   x h ; , .
 */

uint16_t const caps_combo[] PROGMEM = {KC_V, KC_SCLN, COMBO_END}; // Middle fingers

uint16_t const bck_combo[] PROGMEM = {HOME_N, HOME_E, COMBO_END};    // RHS index + middle
uint16_t const esc_combo[] PROGMEM = {HOME_E, HOME_I, COMBO_END}; // RHS middle + ring
uint16_t const ent_combo[] PROGMEM = {HOME_S, HOME_T, COMBO_END};       // LHS index + middle
uint16_t const tab_combo[] PROGMEM = {HOME_R, HOME_S, COMBO_END};       // LHS ring + middle

combo_t key_combos[] = {
    COMBO(caps_combo, CW_TOGG), //
    COMBO(esc_combo, KC_ESC),   //
    COMBO(ent_combo, KC_ENT),   //
    COMBO(bck_combo, KC_BSPC),  //
    COMBO(tab_combo, KC_TAB),   //
};

///////////////////////////////////////////////////////////////////////////////
// Tap-hold configuration (https://docs.qmk.fm/#/tap_hold)
///////////////////////////////////////////////////////////////////////////////

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
        // Ring and pinky fingers are slower, but symbols need to be fast.
        case HOME_R:
        case PNKY_Q:
        case HOME_I:
        case PNKY_DOT:
            return TAPPING_TERM + 15;

        default:
            return TAPPING_TERM;
    }
}

uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t* record) {
    // If you quickly hold a tap-hold key after tapping it, the tap action is
    // repeated. Key repeating is useful e.g. for Vim navigation keys, but can
    // lead to missed triggers in fast typing. Here, returning 0 means we
    // instead want to "force hold" and disable key repeating.
    switch (keycode) {
        case HOME_N:
            return QUICK_TAP_TERM; // Enable key repeating for search.
        default:
            return 0; // Otherwise, force hold and disable key repeating.
    }
}

///////////////////////////////////////////////////////////////////////////////
// Achordion (https://getreuer.info/posts/keyboards/achordion)
///////////////////////////////////////////////////////////////////////////////

bool achordion_chord(uint16_t tap_hold_keycode, keyrecord_t* tap_hold_record, uint16_t other_keycode, keyrecord_t* other_record) {
    // Allow same-hand holds when the other key is on the thumb cluster.
    uint8_t row = other_record->event.key.row;
    if (row == 3 || row == 7) {
        return true;
    }

    return achordion_opposite_hands(tap_hold_record, other_record);
}

bool achordion_eager_mod(uint8_t mod) {
    switch (mod) {
        case MOD_LSFT:
        case MOD_LCTL:
        case MOD_LALT:
            return true; // Eagerly apply left mods for mousing

        default:
            return false;
    }
}

uint16_t achordion_timeout(uint16_t tap_hold_keycode) {
    // Disable achordion for the thumb keys.
    switch (tap_hold_keycode) {
        case THMB_LI:
        case THMB_RI:
            return 0;
        default:
            return 800; // Use a timeout of 800 ms.
    }
}

uint16_t achordion_streak_chord_timeout(uint16_t tap_hold_keycode, uint16_t next_keycode) {
    // Disable streak detection on LT keys.
    if (IS_QK_LAYER_TAP(tap_hold_keycode)) {
        return 0;
    }

    // Exceptions so that certain hotkeys don't get blocked as streaks.
    switch (tap_hold_keycode) {
        case HOME_N:
            if (next_keycode == KC_C || next_keycode == KC_V) {
                return 0;
            }
            break;
    }

    // Otherwise, tap_hold_keycode is a mod-tap key.
    const uint8_t mod = mod_config(QK_MOD_TAP_GET_MODS(tap_hold_keycode));

    if ((mod & MOD_LSFT) != 0) {
        return 100; // A short streak timeout for Shift mod-tap keys.
    } else {
        return 220; // A longer timeout otherwise.
    }
}

///////////////////////////////////////////////////////////////////////////////
// Repeat key (https://docs.qmk.fm/#/feature_repeat_key)
///////////////////////////////////////////////////////////////////////////////

bool remember_last_key_user(uint16_t keycode, keyrecord_t* record, uint8_t* remembered_mods) {
    // Only process the tap part of tap-hold keys.
    switch (keycode) {
        case QK_MOD_TAP ... QK_MOD_TAP_MAX:
            keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
            break;
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
            keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
            break;
    }

    // Forget Shift on most letters when Shift or AltGr are the only mods.
    // This is useful for things like Aaron, etc.
    // Some letters are excluded, e.g. for "NN" and "ZZ" in Vim.
    switch (keycode) {
        case KC_A ... KC_H:
        case KC_K ... KC_M:
        case KC_O ... KC_U:
            if ((*remembered_mods & ~(MOD_MASK_SHIFT | MOD_BIT(KC_RALT))) == 0) {
                *remembered_mods &= ~MOD_MASK_SHIFT;
            }
            break;
    }

    return true;
}

uint16_t get_alt_repeat_key_keycode_user(uint16_t keycode, uint8_t mods) {
    // Only process the tap part of tap-hold keys.
    switch (keycode) {
        case QK_MOD_TAP ... QK_MOD_TAP_MAX:
            keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
            break;
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
            keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
            break;
    }

    // Process special cases, if no mods (other than shift) are active.
    if ((mods & ~MOD_MASK_SHIFT) == 0) {
        switch (keycode) {
            case KC_COMM:
                return M_TDIR; // , -> <BSPC>./
            case KC_HASH:
                return M_INCLUDE; // # -> include
            case KC_DOT:
                if ((mods & MOD_MASK_SHIFT) == 0) {
                    return M_UPDIR; // . -> ./
                }
                break;
            case KC_QUOT:
                if ((mods & MOD_MASK_SHIFT) != 0) {
                    return M_DOCSTR; // " -> ""<cursor>"""
                }
                break;
            case KC_GRV: // ` -> ``<cursor>``` (for Markdown code)
                return M_MKGRVS;
            case KC_TILD:
                return KC_SLSH; // ~ -> /
        }
    }

    // Otherwise, function as a repeat key
    return (((uint16_t)mods) << 8) | keycode;
}

///////////////////////////////////////////////////////////////////////////////
// User macro callbacks (https://docs.qmk.fm/#/feature_macros)
///////////////////////////////////////////////////////////////////////////////

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    // Order is required
    if (!process_achordion(keycode, record)) {
        return false;
    }
    if (!process_custom_shift_keys(keycode, record)) {
        return false;
    }

    if (record->event.pressed) {
        switch (keycode) {
            case STD_CLN:
                SEND_STRING_DELAY("std::", TAP_CODE_DELAY);
                return false;
            case SELLINE:
                SEND_STRING_DELAY(SS_TAP(X_HOME) SS_LSFT(SS_TAP(X_END)), TAP_CODE_DELAY);
                return false;
            case UP_DIR:
                SEND_STRING_DELAY("../", TAP_CODE_DELAY);
                return false;
            case USRNAME:
                SEND_STRING_DELAY("Ross", TAP_CODE_DELAY);
                return false;

                // From the repeat key, break and return true.
            case M_TDIR:
                SEND_STRING_DELAY(/*,*/ SS_TAP(X_BSPC) "./", TAP_CODE_DELAY);
                break;
            case M_UPDIR:
                SEND_STRING_DELAY(/*.*/ "./", TAP_CODE_DELAY);
                set_last_keycode(UP_DIR); // . * * -> ../../
                break;
            case M_DOCSTR:
                SEND_STRING(/*"*/ "\"\"\"\"\"" SS_TAP(X_LEFT) SS_TAP(X_LEFT) SS_TAP(X_LEFT));
                break;
            case M_MKGRVS:
                SEND_STRING(/*`*/ "``\n\n```" SS_TAP(X_UP));
                break;
            case M_INCLUDE:
                SEND_STRING(/*#*/ "include ");
                break;
        }
    }

    return true;
}

void matrix_scan_user(void) {
    achordion_task();
}
