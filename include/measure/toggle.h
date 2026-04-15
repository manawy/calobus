/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include "zephyr/zbus/zbus.h"
#include <stdbool.h>

/* Start a measurement
 *
 * Return true if measurement successfully started
 */
bool start_measurement();

/* Stop a measurement
 *
 * Return false if measurement successfully stopped
 */
bool end_measurement();

/* Toggle the measurement state
 */
void toggle_measurement();

/* Return true if equipment is ready */
bool is_measurement_ready();
/* Call when measurement ready */
void set_measurement_ready();
/* Return true if measurement is on  */
bool is_measurement_on();

ZBUS_CHAN_DECLARE(start_measure_chan);
ZBUS_CHAN_DECLARE(start_trigger_chan);
ZBUS_CHAN_DECLARE(end_measure_chan);
