/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <zephyr/sys/clock.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Start a measurement
 *
 * Return true if measurement successfully started
 */
bool start_measurement();

/* Stop a measurement
 *
 * Return false if measurement successfully stopped
 */
bool stop_measurement();

/* Toggle the measurement state
 */
void toggle_measurement();

/* Return true if equipment is ready */
bool is_measurement_ready();
/* Call when measurement ready */
void toggle_measurement_ready();
/* Return true if measurement is on  */
bool is_measurement_on();
/* Set the measurement interval in ms ("heartbeat")
 *
 * If measurement ongoing, set for next duration
 */
void set_measurement_interval(int interval);
/* Return the current measurement interval (in ms)
 */
int get_measurement_interval();

#ifdef __cplusplus
}
#endif
