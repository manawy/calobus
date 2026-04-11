/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "toggle_measurement.h"
#include "zbus_channels.h"
#include <zephyr/input/input.h>
#include <zephyr/shell/shell.h>

#include "leds_interface.h"

#define HEARTBEAT K_MSEC(CONFIG_HEARTBEAT_MSEC)

LOG_MODULE_REGISTER(toggle_measurement, CONFIG_LOG_DEFAULT_LEVEL);

static bool MeasurementReady = false;
static bool MeasurementOn = false;

void run_trigger() {
    LOG_DBG("run trigger");
    zbus_chan_notify(&start_trigger_chan, K_MSEC(100));
}


K_TIMER_DEFINE(heartbeat_timer, run_trigger, NULL);

bool start_measurement() {
    if (!is_measurement_ready() || is_measurement_on()) {
        return false;
    }
    LOG_PRINTK(" ---- Start measurement ---- \n");
    k_timer_start(&heartbeat_timer, HEARTBEAT, HEARTBEAT);
    zbus_chan_notify(&start_measure_chan, K_MSEC(100));
    MeasurementOn = true;
    return true;
}

bool end_measurement() {
    if (!is_measurement_on()) {
        return false;
    }
    LOG_PRINTK(" ---- Stop measurement ---- \n");
    k_timer_stop(&heartbeat_timer);
    zbus_chan_notify(&end_measure_chan, K_MSEC(100));
    MeasurementOn = false;
    return false;
}

bool is_measurement_ready() {
    return MeasurementReady;
}

bool is_measurement_on() {
    return MeasurementOn;
}

void set_measurement_ready() {
   ledon_start();
   MeasurementReady = true; 
}

static void toggle_measurement() {
    if (!is_measurement_on()) {
        start_measurement();
    } else {
        end_measurement();
    }
}

// ---- Input ----

static void btn_toggle_measurement(struct input_event *evt, void *user_data) {
    if ((evt->code == INPUT_KEY_0) && (evt->value == 0)) {
        toggle_measurement();
    }
}

INPUT_CALLBACK_DEFINE(NULL, btn_toggle_measurement, NULL);

// ---- Shell ---- 

static int start_handler(const struct shell *sh, size_t argc,
                        char **argv) {
    start_measurement();
    return 0;
}
static int end_handler(const struct shell *sh, size_t argc,
                        char **argv) {
    end_measurement();
    return 0;
}
static int status_handler(const struct shell *sh, size_t argc,
                        char **argv) {

    if (is_measurement_on()) {
        shell_print(sh, "Measurement ongoing");
    } else {
        shell_print(sh, "No measurement ongoing");
    }
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_measure,
        SHELL_CMD(start, NULL, "Start measurement.", start_handler),
        SHELL_CMD(stop,   NULL, "Stop measurement.", end_handler),
        SHELL_CMD(status, NULL, "Status of the measurement.", status_handler),
        SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(measure, &sub_measure, "Measure control command", NULL);
