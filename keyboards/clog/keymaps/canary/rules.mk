# Copyright 2024 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Features 
COMBO_ENABLE = yes
CAPS_WORD_ENABLE = yes
EXTRAKEY_ENABLE = yes
LTO_ENABLE = yes
REPEAT_KEY_ENABLE = yes

# Live remapping and key view from the web app (raw HID + dynamic keymap)
VIA_ENABLE = yes

# Disable for space
MOUSEKEY_ENABLE = no
CONSOLE_ENABLE ?= no
GRAVE_ESC_ENABLE ?= no
NKRO_ENABLE ?= no
SPACE_CADET_ENABLE ?= no
TAP_DANCE_ENABLE ?= no

# Achoridion
OPT_DEFS += -DACHORDION_ENABLE
SRC += features/achordion.c

# Custom Shift Keys
OPT_DEFS += -DCUSTOM_SHIFT_KEYS_ENABLE
SRC += features/custom_shift_keys.c
# combo_count() and combo_get() for the combos keymap.c builds
SRC += combos.c
