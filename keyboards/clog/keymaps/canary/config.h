#pragma once

// Unfortunately, some applications drop or miss-order fast key events. This
// is a partial fix to slow down the rate at which macros are sent.
#define TAP_CODE_DELAY 5

// Tap-hold configuration for home row mods.
#define TAPPING_TERM 170
#define TAPPING_TERM_PER_KEY
#define PERMISSIVE_HOLD
#define QUICK_TAP_TERM_PER_KEY

// BASE, SYM, NAV, NUM and the modifier layers CTRL, SHIFT, ALT and GUI.
#define DYNAMIC_KEYMAP_LAYER_COUNT 8

// Combos from the base layer.
#define COMBO_ONLY_FROM_LAYER 0
// The default is 50
#define COMBO_TERM 60

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

// VIA's macro buffer stops 4 bytes short of the end of EEPROM, where keymap.c
// records the shape of the keymap stored, so a flash can keep it.
#define DYNAMIC_KEYMAP_EEPROM_MAX_ADDR (TOTAL_EEPROM_BYTE_COUNT - 5)
