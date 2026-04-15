/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/input/input.h>
#include "measure/toggle.h"

static void btn_toggle_measurement(struct input_event *evt, void *user_data) {
    if ((evt->code == INPUT_KEY_0) && (evt->value == 0)) {
        toggle_measurement();
    }
}

INPUT_CALLBACK_DEFINE(NULL, btn_toggle_measurement, NULL);
