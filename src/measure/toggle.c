/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "measure/toggle.h"
#include "zbus_channels.h"

#include "interface/leds.h"

#define HEARTBEAT K_MSEC(CONFIG_HEARTBEAT_MSEC)

static bool MeasurementReady = false;
static bool MeasurementOn = false;

void run_trigger() {
    zbus_chan_notify(&start_trigger_chan, K_MSEC(100));
}


K_TIMER_DEFINE(heartbeat_timer, run_trigger, NULL);

bool start_measurement() {
    if (!is_measurement_ready() || is_measurement_on()) {
        return false;
    }
    k_timer_start(&heartbeat_timer, HEARTBEAT, HEARTBEAT);
    zbus_chan_notify(&start_measure_chan, K_MSEC(100));
    MeasurementOn = true;
    return true;
}

bool end_measurement() {
    if (!is_measurement_on()) {
        return false;
    }
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

void toggle_measurement() {
    if (!is_measurement_on()) {
        start_measurement();
    } else {
        end_measurement();
    }
}

