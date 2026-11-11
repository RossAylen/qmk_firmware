#include QMK_KEYBOARD_H

#include "eeprom.h"
#include "features/achordion.h"
#include "features/custom_shift_keys.h"

#ifndef VIA_ENABLE
#    error "The layout is set from the web app over VIA's raw HID: VIA_ENABLE must be on"
#endif
#include "raw_hid.h"

/**
 * This firmware holds no layout. The keymap is VIA's, in EEPROM, and the rest
 * of the layout (combos, tapping terms, Shift and Alt-Repeat pairs, what each
 * layer is called and the modifiers it holds) is a block at the end of EEPROM
 * below. Both are written from the web app, Clog Studio; a blank keyboard types
 * nothing until it does.
 */

void process_record_live(uint16_t keycode, keyrecord_t* record);

///////////////////////////////////////////////////////////////////////////////
// Keymap
///////////////////////////////////////////////////////////////////////////////

enum custom_keycodes {
    ALT_TAB = SAFE_RANGE, // Alt+Tab, Alt held until its layer is released
};

// clang-format off

// One layer of nothing, which VIA copies to a blank EEPROM. The layers past it
// come out transparent.
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x5_2(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,

                                   XXXXXXX, XXXXXXX,      XXXXXXX, XXXXXXX
    ),
};

// clang-format on

///////////////////////////////////////////////////////////////////////////////
// The rest of the layout, in EEPROM
///////////////////////////////////////////////////////////////////////////////

/**
 * CLOG_CONFIG_SIZE bytes from just after VIA's macro buffer (config.h), read
 * and written by the web app with id_config_read and id_config_write, offsets
 * big-endian as VIA's own; the 16-bit values inside are little-endian. A key is
 * named by its matrix index, row * MATRIX_COLS + col. Zero means none or
 * default throughout, so a zeroed block is an empty layout, and anything but
 * CONFIG_VERSION in the first byte reads as one.
 */
#define CONFIG_VERSION 1
#define CONFIG_KEYS (MATRIX_ROWS * MATRIX_COLS)
#define CONFIG_NAME_SIZE 5 // ASCII, padded with 0
#define CONFIG_COMBOS 10
#define CONFIG_COMBO_KEYS 3
#define CONFIG_NO_KEY 0xFF
#define CONFIG_SHIFTS 10
#define CONFIG_ALTS 10

// A key's timing: bits 0-6 its tapping term in steps of 5 ms (0 for the
// layout's), and bit 7 a quick tap, so tapping it and then holding it repeats
// its tap rather than holding.
#define TIMING_TERM 0x7F
#define TIMING_TERM_STEP 5
#define TIMING_QUICK_TAP 0x80

// When an Alt-Repeat pair applies: whether the last key was typed with Shift.
enum alt_shift {
    ALT_EITHER = 0,
    ALT_WITHOUT_SHIFT,
    ALT_WITH_SHIFT,
};

typedef struct PACKED {
    uint8_t  keys[CONFIG_COMBO_KEYS]; // pressed together, CONFIG_NO_KEY for none
    uint16_t keycode;                 // what they send
} config_combo_t;

typedef struct PACKED {
    uint16_t from; // a keycode, or a tap-hold key's tap keycode
    uint16_t to;   // what it sends with Shift instead
} config_shift_t;

typedef struct PACKED {
    uint16_t from; // the last key typed, as a tap keycode
    uint8_t  shift; // enum alt_shift
    uint16_t to;    // what Alt-Repeat sends after it
} config_alt_t;

typedef struct PACKED {
    uint8_t        version;
    uint16_t       tapping_term; // ms
    uint8_t        combo_term;   // ms
    uint8_t        layer_mods[DYNAMIC_KEYMAP_LAYER_COUNT]; // get_mods() bits each layer holds while on
    char           layer_names[DYNAMIC_KEYMAP_LAYER_COUNT][CONFIG_NAME_SIZE]; // only for the app
    uint8_t        key_timing[CONFIG_KEYS];
    config_combo_t combos[CONFIG_COMBOS];
    config_shift_t shift[CONFIG_SHIFTS];
    config_alt_t   alt[CONFIG_ALTS];
} config_t;

STATIC_ASSERT(sizeof(config_t) == CLOG_CONFIG_SIZE, "config.h's CLOG_CONFIG_SIZE must match config_t");

#define CONFIG_EEPROM ((uint8_t*)(DYNAMIC_KEYMAP_EEPROM_MAX_ADDR + 1))

static config_t config;       // the block, or all zeros while it isn't valid
static bool     config_valid; // whether the EEPROM holds a layout
static bool     combos_stale; // the base layer changed under the combos

// No combos built in; load_combos() builds them, and combos.c hands them to QMK.
combo_t         key_combos[] = {};
combo_t         clog_combos[CONFIG_COMBOS];
uint8_t         clog_combo_count;
static uint16_t combo_keys[CONFIG_COMBOS][CONFIG_COMBO_KEYS + 1];

// QMK matches combos on keycodes from the base layer (COMBO_ONLY_FROM_LAYER),
// and the layout names keys, so each combo takes the keycodes its keys have
// there now. A key with nothing on the base layer leaves the combo out.
static void load_combos(void) {
    clog_combo_count = 0;
    for (uint8_t i = 0; i < CONFIG_COMBOS; i++) {
        const config_combo_t* c = &config.combos[i];
        uint8_t               n = 0;

        if (c->keycode == KC_NO) {
            continue;
        }
        for (uint8_t k = 0; k < CONFIG_COMBO_KEYS; k++) {
            if (c->keys[k] >= CONFIG_KEYS) {
                continue;
            }
            uint16_t keycode = dynamic_keymap_get_keycode(0, c->keys[k] / MATRIX_COLS, c->keys[k] % MATRIX_COLS);
            if (keycode != KC_NO && keycode != KC_TRNS) {
                combo_keys[clog_combo_count][n++] = keycode;
            }
        }
        if (n < 2) {
            continue;
        }
        combo_keys[clog_combo_count][n] = COMBO_END;
        clog_combos[clog_combo_count]  = (combo_t){.keys = combo_keys[clog_combo_count], .keycode = c->keycode};
        clog_combo_count++;
    }
    combos_stale = false;
}

static void load_config(void) {
    eeprom_read_block(&config, CONFIG_EEPROM, sizeof(config));
    config_valid = config.version == CONFIG_VERSION;
    if (!config_valid) {
        memset(&config, 0, sizeof(config));
    }
    load_combos();
}

// Called when the keymap was reset, so the two never disagree.
static void clear_config(void) {
    eeprom_update_byte(CONFIG_EEPROM, 0);
}

uint16_t get_combo_term(uint16_t index, combo_t* combo) {
    return config.combo_term ? config.combo_term : COMBO_TERM;
}

void housekeeping_task_user(void) {
    if (combos_stale) {
        load_combos();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Modifier layers
///////////////////////////////////////////////////////////////////////////////

// A layer can hold modifiers down while it is on, as a home-row Ctrl does: a
// key left transparent on it falls through to the layer below and goes with
// the modifier, hold T and press O for Ctrl+O, or NAV's left arrow for
// Ctrl+Left. A key changed on it sends just what it is changed to, as a key on
// a symbol layer does.
static uint8_t      layer_mods_on;           // what the modifier layers have added to get_mods()
static matrix_row_t own_keys[MATRIX_ROWS]; // keys down that a modifier layer changed

static uint8_t mods_of_layer(uint8_t layer) {
    return layer < DYNAMIC_KEYMAP_LAYER_COUNT ? config.layer_mods[layer] : 0;
}

// Hold down the modifiers of the layers on in `state`, and none while a key
// changed on one of them is down.
static void update_layer_mods(layer_state_t state) {
    uint8_t want = 0;
    bool    own  = false;

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        own |= own_keys[row] != 0;
    }
    for (uint8_t layer = 0; layer < DYNAMIC_KEYMAP_LAYER_COUNT && !own; layer++) {
        if (state & ((layer_state_t)1 << layer)) {
            want |= mods_of_layer(layer);
        }
    }
    if (want != layer_mods_on) {
        del_mods(layer_mods_on & ~want);
        add_mods(want & ~layer_mods_on);
        layer_mods_on = want;
        send_keyboard_report();
    }
}

static void track_own_keys(keyrecord_t* record) {
    keypos_t key = record->event.key;

    if (!IS_KEYEVENT(record->event) || key.row >= MATRIX_ROWS || key.col >= MATRIX_COLS) {
        return;
    }
    matrix_row_t bit = (matrix_row_t)1 << key.col;
    if (record->event.pressed) {
        if (!mods_of_layer(layer_switch_get_layer(key))) {
            return;
        }
        own_keys[key.row] |= bit;
    } else {
        if (!(own_keys[key.row] & bit)) {
            return;
        }
        own_keys[key.row] &= ~bit;
    }
    update_layer_mods(layer_state);
}

///////////////////////////////////////////////////////////////////////////////
// Custom shift keys (https://getreuer.info/posts/keyboards/custom-shift-keys)
///////////////////////////////////////////////////////////////////////////////

uint16_t get_custom_shift_keycode(uint16_t keycode) {
    for (uint8_t i = 0; i < CONFIG_SHIFTS; i++) {
        if (keycode != KC_NO && config.shift[i].from == keycode) {
            return config.shift[i].to;
        }
    }
    return KC_NO;
}

///////////////////////////////////////////////////////////////////////////////
// Tap-hold configuration (https://docs.qmk.fm/#/tap_hold)
///////////////////////////////////////////////////////////////////////////////

static uint8_t key_timing(keyrecord_t* record) {
    keypos_t key = record->event.key;

    if (!IS_KEYEVENT(record->event) || key.row >= MATRIX_ROWS || key.col >= MATRIX_COLS) {
        return 0;
    }
    return config.key_timing[key.row * MATRIX_COLS + key.col];
}

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t* record) {
    uint8_t steps = key_timing(record) & TIMING_TERM;

    if (steps) {
        return steps * TIMING_TERM_STEP;
    }
    return config.tapping_term ? config.tapping_term : TAPPING_TERM;
}

// If you quickly hold a tap-hold key after tapping it, the tap action is
// repeated: useful on a key you hold to repeat, such as one for Vim's search,
// but it misses holds in fast typing, so it is off unless the layout asks.
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t* record) {
    return key_timing(record) & TIMING_QUICK_TAP ? get_tapping_term(keycode, record) : 0;
}

///////////////////////////////////////////////////////////////////////////////
// Achordion (https://getreuer.info/posts/keyboards/achordion)
///////////////////////////////////////////////////////////////////////////////

static bool on_thumb(keyrecord_t* record) {
    uint8_t row = record->event.key.row;
    return row == 3 || row == 7;
}

// A tap-hold key holds for a key on the other hand or a thumb, and a held
// thumb key holds for any key.
bool achordion_chord(uint16_t tap_hold_keycode, keyrecord_t* tap_hold_record, uint16_t other_keycode, keyrecord_t* other_record) {
    if (on_thumb(other_record) || on_thumb(tap_hold_record)) {
        return true;
    }
    return achordion_opposite_hands(tap_hold_record, other_record);
}

// A mod-tap key sends no modifier until the next key settles the hold, as the
// modifier layers don't.
bool achordion_eager_mod(uint8_t mod) {
    return false;
}

uint16_t achordion_streak_chord_timeout(uint16_t tap_hold_keycode, uint16_t next_keycode) {
    // The modifiers the key holds: its modifier layer's, or a mod-tap key's own
    // (5-bit, whose low nibble is the left hand's get_mods() bits for either hand).
    uint8_t mods = 0;
    if (IS_QK_LAYER_TAP(tap_hold_keycode)) {
        mods = mods_of_layer(QK_LAYER_TAP_GET_LAYER(tap_hold_keycode));
    } else if (IS_QK_MOD_TAP(tap_hold_keycode)) {
        mods = mod_config(QK_MOD_TAP_GET_MODS(tap_hold_keycode)) & 0x0F;
    }
    if (IS_QK_MOD_TAP(next_keycode)) {
        next_keycode = QK_MOD_TAP_GET_TAP_KEYCODE(next_keycode);
    } else if (IS_QK_LAYER_TAP(next_keycode)) {
        next_keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(next_keycode);
    }

    if (mods == 0) {
        return 0; // No streak detection for a plain layer.
    } else if ((mods & ~MOD_MASK_CTRL) == 0 && (next_keycode == KC_C || next_keycode == KC_V)) {
        return 0; // Copy and paste straight after typing.
    } else if ((mods & MOD_MASK_SHIFT) != 0) {
        return 100; // A short streak timeout for Shift.
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

static bool alt_from_pair; // Alt-Repeat is sending one of the layout's pairs

// Alt-Repeat sends the layout's pair for the last key, if it has one and no
// modifier but Shift was held, and otherwise repeats the last key.
uint16_t get_alt_repeat_key_keycode_user(uint16_t keycode, uint8_t mods) {
    switch (keycode) {
        case QK_MOD_TAP ... QK_MOD_TAP_MAX:
            keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
            break;
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
            keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
            break;
    }

    alt_from_pair = false;
    if ((mods & ~MOD_MASK_SHIFT) == 0) {
        uint8_t shift = mods & MOD_MASK_SHIFT ? ALT_WITH_SHIFT : ALT_WITHOUT_SHIFT;

        for (uint8_t i = 0; i < CONFIG_ALTS; i++) {
            const config_alt_t* a = &config.alt[i];
            if (keycode != KC_NO && a->from == keycode && (a->shift == ALT_EITHER || a->shift == shift)) {
                alt_from_pair = true;
                return a->to;
            }
        }
    }

    if (!IS_QK_BASIC(keycode)) {
        return keycode;
    }
    // The 8-bit mods in a keycode's 5 bits, as get_alt_repeat_key_keycode()
    // does: mixed hands become the right.
    uint8_t mods5 = ((mods & 0xF0) ? 0x10 : 0) | (((mods >> 4) | mods) & 0x0F);
    return ((uint16_t)mods5 << 8) | keycode;
}

///////////////////////////////////////////////////////////////////////////////
// User macro callbacks (https://docs.qmk.fm/#/feature_macros)
///////////////////////////////////////////////////////////////////////////////

// Set while ALT_TAB is holding Alt down, with the layer it was pressed on.
static bool    alt_tab_active = false;
static uint8_t alt_tab_layer;

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    // An Alt-Repeat pair's keycode becomes the last key, so Alt-Repeat again
    // goes on from it: with . to ./ and ./ to ../, . Alt-Repeat Alt-Repeat
    // types ../../.
    if (alt_from_pair && get_repeat_key_count() < 0 && record->event.pressed) {
        alt_from_pair = false;
        set_last_keycode(keycode);
        set_last_mods(0);
    }

    // Order is required
    if (!process_achordion(keycode, record)) {
        return false;
    }
    track_own_keys(record);
    process_record_live(keycode, record);
    if (!process_custom_shift_keys(keycode, record)) {
        return false;
    }

    // Alt stays down after the first tap, so tapping again steps on through
    // the windows instead of flipping between the last two (Shift steps back).
    // It is let go with the layer the key is on, or with the key itself on the
    // base layer.
    if (keycode == ALT_TAB) {
        if (record->event.pressed) {
            if (!alt_tab_active) {
                alt_tab_active = true;
                alt_tab_layer  = IS_KEYEVENT(record->event) ? layer_switch_get_layer(record->event.key) : 0;
                register_code(KC_LALT);
            }
            register_code(KC_TAB);
        } else {
            unregister_code(KC_TAB);
            if (alt_tab_layer == 0) {
                unregister_code(KC_LALT);
                alt_tab_active = false;
            }
        }
        return false;
    }

    return true;
}

void matrix_scan_user(void) {
    achordion_task();
}

layer_state_t layer_state_set_user(layer_state_t state) {
    if (alt_tab_active && alt_tab_layer != 0 && !layer_state_cmp(state, alt_tab_layer)) {
        unregister_code(KC_LALT);
        alt_tab_active = false;
    }
    update_layer_mods(state);
    return state;
}

///////////////////////////////////////////////////////////////////////////////
// Live view for the web remapper (raw HID, polled by the host)
///////////////////////////////////////////////////////////////////////////////

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
    // 0xF5 was keymap.c's keymaps[], when keymap.c had a layout.
    id_config_info  = 0xF6, // the layout block's version, size and table sizes, and whether it holds a layout
    id_config_read  = 0xF7, // offset (big-endian), size; the bytes from data[4]
    id_config_write = 0xF8, // the same, written; replies with what it wrote
};

/**
 * The layout in EEPROM outlives a flash. VIA resets its EEPROM (the keymap and
 * macros) whenever the build date changes, so via_init_kb() marks it valid for
 * this build first, if what's there was stored in the same place and shape. A
 * QMK update that moves VIA's EEPROM, a new layer count or renumbered keycodes
 * changes the shape, and then via_init_kb() marks it invalid: VIA's magic is
 * only the build's day, so a build of another shape on the same day would
 * otherwise keep it, and read a new layer out of the old macro space. VIA's own
 * reset command, or a reset cut short, leaves its magic at 0xFF, and that still
 * resets too. Whenever the keymap resets, the layout block goes with it, so the
 * app finds the keyboard blank and writes the whole layout back.
 */
#    include "eeprom.h"
#    include "nvm_via.h"
#    include "nvm_eeprom_eeconfig_internal.h"
#    include "nvm_eeprom_via_internal.h" // VIA_EEPROM_CONFIG_END, where the dynamic keymap starts

#    define KEYMAP_SHAPE_ADDR ((uint32_t*)(TOTAL_EEPROM_BYTE_COUNT - 4)) // the last 4 bytes, kept from VIA by config.h
#    define KEYMAP_SHAPE ((uint32_t)(VIA_EEPROM_CONFIG_END) << 20 | (uint32_t)DYNAMIC_KEYMAP_LAYER_COUNT << 16 | (MATRIX_ROWS * MATRIX_COLS) << 8 | (QMK_KEYCODES_VERSION_BCD & 0xFF))

void via_init_kb(void) {
    uint8_t magic0, magic1, magic2;

    nvm_via_read_magic(&magic0, &magic1, &magic2);
    bool valid = eeprom_read_dword(KEYMAP_SHAPE_ADDR) == KEYMAP_SHAPE && magic0 != 0xFF;
    via_eeprom_set_valid(valid);
    if (!valid) {
        clear_config();
    }
}

// A blank EEPROM, or QK_CLEAR_EEPROM.
void eeconfig_init_user(void) {
    eeconfig_update_user(0);
    clear_config();
}

// After VIA and eeconfig have reset whatever they were going to.
void keyboard_post_init_user(void) {
    eeprom_update_dword(KEYMAP_SHAPE_ADDR, KEYMAP_SHAPE);
    load_config();
}

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
        // VIA's own keymap writes, left to VIA: the combos take their keycodes
        // from the base layer, so they are rebuilt after.
        case id_dynamic_keymap_set_keycode:
        case id_dynamic_keymap_set_buffer:
            combos_stale = true;
            return false;

        // A reset keymap is a blank one, and takes the rest of the layout with it.
        case id_dynamic_keymap_reset:
            clear_config();
            load_config();
            return false;

        case id_config_info:
            data[1]  = CONFIG_VERSION;
            data[2]  = sizeof(config_t) & 0xFF;
            data[3]  = sizeof(config_t) >> 8;
            data[4]  = CONFIG_COMBOS;
            data[5]  = CONFIG_COMBO_KEYS;
            data[6]  = CONFIG_SHIFTS;
            data[7]  = CONFIG_ALTS;
            data[8]  = CONFIG_NAME_SIZE;
            data[9]  = config_valid;
            data[10] = DYNAMIC_KEYMAP_LAYER_COUNT;
            data[11] = CONFIG_KEYS;
            break;

        case id_config_read:
        case id_config_write: {
            uint16_t offset = (data[1] << 8) | data[2];
            uint8_t  size   = data[3] < 28 ? data[3] : 28;

            if (offset > sizeof(config_t)) {
                offset = sizeof(config_t);
            }
            if (size > sizeof(config_t) - offset) {
                size = sizeof(config_t) - offset;
            }
            data[3] = size;
            if (data[0] == id_config_write) {
                eeprom_update_block(&data[4], CONFIG_EEPROM + offset, size);
                load_config();
            } else {
                eeprom_read_block(&data[4], CONFIG_EEPROM + offset, size);
            }
            break;
        }

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
