#include QMK_KEYBOARD_H

#include "features/achordion.h"
#include "features/custom_shift_keys.h"

#ifdef VIA_ENABLE
#    include "raw_hid.h"
#endif

void process_record_live(uint16_t keycode, keyrecord_t* record);

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
    ALT_TAB,   // Alt+Tab, Alt held until NAV is released
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
        C(KC_C), ALT_TAB,    KC_LSFT, KC_LCTL, C(KC_SLSH),       KC_PGUP, KC_LEFT,   KC_UP, KC_RGHT, XXXXXXX,
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
uint16_t const ent_combo[] PROGMEM = {HOME_E, HOME_I, COMBO_END}; // RHS middle + ring
uint16_t const esc_combo[] PROGMEM = {HOME_R, HOME_S, COMBO_END};       // LHS ring + middle
uint16_t const tab_combo[] PROGMEM = {HOME_S, HOME_T, COMBO_END};       // LHS index + middle

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
    // Disable achordion for the thumb keys. ACHORDION_NO_TIMEOUT means any
    // other value only switches it on.
    switch (tap_hold_keycode) {
        case THMB_LI:
        case THMB_RI:
            return 0;
        default:
            return 1;
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

// Set while ALT_TAB is holding Alt down.
static bool alt_tab_active = false;

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    // Order is required
    if (!process_achordion(keycode, record)) {
        return false;
    }
    process_record_live(keycode, record);
    if (!process_custom_shift_keys(keycode, record)) {
        return false;
    }

    // Alt stays down after the first tap, so tapping again steps on through
    // the windows instead of flipping between the last two (NAV's Shift steps
    // back). layer_state_set_user lets go of Alt when NAV is released.
    if (keycode == ALT_TAB) {
        if (record->event.pressed) {
            if (!alt_tab_active) {
                alt_tab_active = true;
                register_code(KC_LALT);
            }
            register_code(KC_TAB);
        } else {
            unregister_code(KC_TAB);
        }
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

layer_state_t layer_state_set_user(layer_state_t state) {
    if (alt_tab_active && !layer_state_cmp(state, NAV)) {
        unregister_code(KC_LALT);
        alt_tab_active = false;
    }
    return state;
}

///////////////////////////////////////////////////////////////////////////////
// Live view for the web remapper (raw HID, polled by the host)
///////////////////////////////////////////////////////////////////////////////

#ifdef VIA_ENABLE

/**
 * The host polls; the keyboard never sends unprompted. raw_hid_send() waits up
 * to ~10 ms for the host to read, and nothing reads the raw HID endpoint while
 * the web app is closed, so pushing events would stall the scan loop.
 *
 * Events go into a ring indexed by an 8-bit sequence number. The host asks for
 * events from the next sequence it expects; if it fell more than a ring behind,
 * the reply starts at the oldest event still held and the host sees the gap.
 */

// Command ids sit above VIA's own (0x01-0x14) and below id_unhandled (0xFF).
enum live_command_ids {
    id_live_get_state  = 0xF0,
    id_live_get_events = 0xF1,
    id_live_bootloader = 0xF2,
    id_live_read_flash = 0xF3, // for finding out what the bootloader allows
    id_live_read_fuses = 0xF4,
    id_live_get_keymap_c = 0xF5, // keymap.c's keymaps[], which a blank EEPROM keymap starts from
};

/**
 * The keymap is the one in EEPROM, edited from the web app, and outlives a
 * flash; keymaps[] above only fills a blank EEPROM. VIA resets the EEPROM to
 * keymaps[] whenever the build date changes, so via_init_kb() marks it valid
 * for this build first, if what's there was stored in the same place and
 * shape. A QMK update that moves VIA's EEPROM, a new layer count or renumbered
 * keycodes changes the shape, and VIA resets as it would have. VIA's own reset
 * command, or a reset cut short, leaves its magic at 0xFF, and that still
 * resets too.
 */
#    include "eeprom.h"
#    include "nvm_via.h"
#    include "nvm_eeprom_eeconfig_internal.h"
#    include "nvm_eeprom_via_internal.h" // VIA_EEPROM_CONFIG_END, where the dynamic keymap starts

#    define KEYMAP_SHAPE_ADDR ((uint32_t*)(DYNAMIC_KEYMAP_EEPROM_MAX_ADDR + 1)) // the 4 bytes config.h keeps from VIA's macros
#    define KEYMAP_SHAPE ((uint32_t)(VIA_EEPROM_CONFIG_END) << 20 | (uint32_t)DYNAMIC_KEYMAP_LAYER_COUNT << 16 | (MATRIX_ROWS * MATRIX_COLS) << 8 | (QMK_KEYCODES_VERSION_BCD & 0xFF))

void via_init_kb(void) {
    uint8_t magic0, magic1, magic2;

    nvm_via_read_magic(&magic0, &magic1, &magic2);
    if (eeprom_read_dword(KEYMAP_SHAPE_ADDR) == KEYMAP_SHAPE && magic0 != 0xFF) {
        via_eeprom_set_valid(true);
    }
}

// After VIA and eeconfig have reset whatever they were going to.
void keyboard_post_init_user(void) {
    eeprom_update_dword(KEYMAP_SHAPE_ADDR, KEYMAP_SHAPE);
}

#    include "keymap_introspection.h"

#    ifdef __AVR__
#        include <avr/boot.h>
#        include <avr/pgmspace.h>
#    endif

#    define LIVE_EVENT_COUNT 64 // power of two, so the ring index survives the sequence wrapping; 448 bytes of RAM
#    define LIVE_EVENT_SIZE 7
#    define LIVE_EVENTS_PER_REPLY 4 // (32 - 4 header bytes) / 7

// Event flags
#    define LIVE_PRESSED 0x80
#    define LIVE_PROCESSED 0x40 // as QMK acted on it, after combos, tap-hold and Achordion
#    define LIVE_COMBO 0x20
#    define LIVE_TAP_COUNT 0x0F

typedef struct {
    uint8_t  flags;
    uint8_t  row;
    uint8_t  col;
    uint16_t keycode;
    uint16_t time;
} live_event_t;

static live_event_t live_events[LIVE_EVENT_COUNT];
static uint8_t      live_head;  // sequence number of the next event to be written
static uint8_t      live_count; // events held, up to LIVE_EVENT_COUNT, so a fresh ring never replays empty slots

static void live_record(uint16_t keycode, keyrecord_t* record, uint8_t source) {
    live_event_t* e = &live_events[live_head % LIVE_EVENT_COUNT];

    e->flags   = source | (record->event.pressed ? LIVE_PRESSED : 0) | (IS_COMBOEVENT(record->event) ? LIVE_COMBO : 0) | (record->tap.count & LIVE_TAP_COUNT);
    e->row     = record->event.key.row;
    e->col     = record->event.key.col;
    e->keycode = keycode;
    e->time    = record->event.time;
    live_head++;
    if (live_count < LIVE_EVENT_COUNT) {
        live_count++;
    }
}

// Physical presses and releases, before combos and tap-hold see them.
bool pre_process_record_user(uint16_t keycode, keyrecord_t* record) {
    live_record(keycode, record, 0);
    return true;
}

void process_record_live(uint16_t keycode, keyrecord_t* record) {
    live_record(keycode, record, LIVE_PROCESSED);
}

bool via_command_kb(uint8_t* data, uint8_t length) {
    switch (data[0]) {
        case id_live_get_state: {
            uint32_t layers = layer_state;
            uint16_t now    = timer_read();

            data[1] = is_caps_word_on() ? 1 : 0;
            data[2] = layers & 0xFF;
            data[3] = (layers >> 8) & 0xFF;
            data[4] = (layers >> 16) & 0xFF;
            data[5] = (layers >> 24) & 0xFF;
            data[6] = default_layer_state & 0xFF;
            data[7] = get_mods();
            data[8] = get_oneshot_mods();
            data[9] = get_weak_mods();
            data[10] = MATRIX_ROWS;
            for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
                data[11 + row] = matrix_get_row(row);
            }
            data[11 + MATRIX_ROWS] = live_head;
            data[12 + MATRIX_ROWS] = now & 0xFF;
            data[13 + MATRIX_ROWS] = now >> 8;
            break;
        }

        case id_live_get_events: {
            uint8_t from      = data[1];
            uint8_t available = live_head - from;

            if (available > live_count) {
                from      = live_head - live_count;
                available = live_count;
            }
            uint8_t count = available < LIVE_EVENTS_PER_REPLY ? available : LIVE_EVENTS_PER_REPLY;

            data[1] = live_head;
            data[2] = from;
            data[3] = count;
            for (uint8_t i = 0; i < count; i++) {
                live_event_t* e = &live_events[(uint8_t)(from + i) % LIVE_EVENT_COUNT];
                uint8_t*      d = &data[4 + i * LIVE_EVENT_SIZE];

                d[0] = e->flags;
                d[1] = e->row;
                d[2] = e->col;
                d[3] = e->keycode & 0xFF;
                d[4] = e->keycode >> 8;
                d[5] = e->time & 0xFF;
                d[6] = e->time >> 8;
            }
            break;
        }

        // keymap.c's own keycodes, laid out as id_dynamic_keymap_get_buffer
        // reads the EEPROM's (big-endian, layer by row by column), so the host
        // can tell a remap made in the app from a change to keymap.c.
        case id_live_get_keymap_c: {
            uint16_t offset = (data[1] << 8) | data[2];
            uint8_t  size   = data[3] < 28 ? data[3] : 28;

            for (uint8_t i = 0; i < size; i++) {
                uint16_t n    = (offset + i) / 2;
                uint16_t code = keycode_at_keymap_location_raw(n / (MATRIX_ROWS * MATRIX_COLS), (n / MATRIX_COLS) % MATRIX_ROWS, n % MATRIX_COLS);
                data[4 + i]   = (offset + i) & 1 ? code & 0xFF : code >> 8;
            }
            break;
        }

#    ifdef __AVR__
        // Flash bytes by LPM from the application section, which the boot
        // lock bits may refuse for the bootloader's own pages (reads 0xFF then).
        case id_live_read_flash: {
            uint16_t address = data[1] | (data[2] << 8);
            uint8_t  size    = data[3] < 28 ? data[3] : 28;

            for (uint8_t i = 0; i < size; i++) {
                data[4 + i] = pgm_read_byte((uint16_t)(address + i));
            }
            break;
        }

        // Fuses, lock bits and signature: readable from software whatever the lock bits say.
        case id_live_read_fuses:
            data[1] = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
            data[2] = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
            data[3] = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
            data[4] = boot_lock_fuse_bits_get(GET_LOCK_BITS);
            data[5] = boot_signature_byte_get(0x0000);
            data[6] = boot_signature_byte_get(0x0002);
            data[7] = boot_signature_byte_get(0x0004);
            break;
#    endif

        case id_live_bootloader:
            // Reply first so the host knows the command landed before the device drops off the bus.
            raw_hid_send(data, length);
            wait_ms(100);
            bootloader_jump();
            return true;

        default:
            return false;
    }

    raw_hid_send(data, length);
    return true;
}

#else

void process_record_live(uint16_t keycode, keyrecord_t* record) {}

#endif // VIA_ENABLE
