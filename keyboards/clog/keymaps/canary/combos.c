// QMK's combos, from the ones keymap.c builds out of the layout in EEPROM.
// These can't be in keymap.c: QMK compiles it into keymap_introspection.c,
// beside the weak defaults they override.

#include "quantum.h"

extern combo_t clog_combos[];
extern uint8_t clog_combo_count;

uint16_t combo_count(void) {
    return clog_combo_count;
}

combo_t* combo_get(uint16_t index) {
    return &clog_combos[index];
}
