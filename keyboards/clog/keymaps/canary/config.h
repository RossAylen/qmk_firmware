#pragma once

// Unfortunately, some applications drop or miss-order fast key events. This
// is a partial fix to slow down the rate at which macros are sent.
#define TAP_CODE_DELAY 5

// Tap-hold configuration. The tapping terms are the layout's, in EEPROM
// (keymap.c), and QMK's own default of 200 ms holds until it has one.
#define TAPPING_TERM_PER_KEY
#define PERMISSIVE_HOLD
#define QUICK_TAP_TERM_PER_KEY

// Eight layers, all set from the web app: what they are called and which
// modifier each holds is in the layout's EEPROM block (keymap.c).
#define DYNAMIC_KEYMAP_LAYER_COUNT 8

// Combos are keys on the base layer pressed together, and keymap.c builds them
// from the layout in EEPROM, so their key lists are in RAM.
#define COMBO_ONLY_FROM_LAYER 0
#define COMBO_KEYS_IN_RAM
#define COMBO_TERM_PER_COMBO

// Enable the streak feature
#define ACHORDION_STREAK
// A tap-hold key held with a same-hand key is always a tap, however long it is
// held: the hold is for the other hand (achordion_chord). See achordion.c.
#define ACHORDION_NO_TIMEOUT

// A tap-hold key held and let go without another key does nothing, as the SYM
// keys always did, instead of sending its Ctrl, Shift, Alt or GUI on its own
// (a lone GUI opens the GNOME overview, a lone Alt the menu bar). See achordion.c.
#define ACHORDION_LONE_HOLD_DOES_NOTHING

// Holding Shift while Caps Word is active inverts the shift state.
#define CAPS_WORD_INVERT_ON_SHIFT
// When idle, turn off Caps Word after 5 seconds.
#define CAPS_WORD_IDLE_TIMEOUT 5000

// Don't apply custom shift keys with mods other than Shift.
#define CUSTOM_SHIFT_KEYS_NEGMODS ~MOD_MASK_SHIFT

// A USB serial number, so Chrome remembers the Clog Studio page's WebHID
// permission across replugs and flashes; it forgets devices without one. The
// same on both halves, so either one on the cable is the same keyboard.
#define SERIAL_NUMBER "clog"

// The end of EEPROM, after VIA's macro buffer, holds the rest of the layout
// (combos, timings, Shift and Alt-Repeat pairs, layer names and modifiers) and,
// in its last 4 bytes, the shape of the keymap stored, so a flash can keep it.
// See keymap.c.
#define CLOG_CONFIG_SIZE 232
#define DYNAMIC_KEYMAP_EEPROM_MAX_ADDR (TOTAL_EEPROM_BYTE_COUNT - 4 - CLOG_CONFIG_SIZE - 1)
