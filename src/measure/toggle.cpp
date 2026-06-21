/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "measure/toggle.h"
#include "zbus_channels.h"

LOG_MODULE_REGISTER(toggle, CONFIG_LOG_DEFAULT_LEVEL);

constexpr k_timeout_t ZBUS_TIMEOUT = K_MSEC(50);

class MeasurementState
{
public:
    MeasurementState():
        measurement_ready(false),
        measurement_on(false),
        m_heartbeat_duration_ms(CONFIG_HEARTBEAT_MSEC)
    {
        k_timer_init(&m_heartbeat, run_trigger, NULL);
    }

    static void run_trigger(struct k_timer*) {
        LOG_INF("Triggered !");
        zbus_chan_notify(&start_trigger_chan, ZBUS_TIMEOUT);
    }

    bool start_measurement();
    bool stop_measurement();

    bool is_measurement_ready() const {
        return measurement_ready;
    }

    bool is_measurement_on() const {
        return measurement_on;
    }

    bool toggle_measurement() {
        if (is_measurement_on()) 
            return stop_measurement();
        return start_measurement();
    };

    void toggle_measurement_ready() {
        measurement_ready = !measurement_ready;
    }

    // Set the heartbeat duration
    //
    // If measurement ongoing, value will be set for next measurement
    void set_heartbeat(int heartbeat);

    int get_heartbeat() {
        return m_heartbeat_duration_ms;
    }

private:
    bool measurement_ready;
    bool measurement_on;
    struct k_timer m_heartbeat;
    int m_heartbeat_duration_ms;
};


bool MeasurementState::start_measurement() 
{
    if (!is_measurement_ready() || is_measurement_on()) {
        return false;
    }
    zbus_chan_notify(&start_measure_chan, ZBUS_TIMEOUT);
    k_timer_start(&m_heartbeat, K_MSEC(m_heartbeat_duration_ms), K_MSEC(m_heartbeat_duration_ms));
    measurement_ready = false;
    measurement_on = true;
    return measurement_on;
}

bool MeasurementState::stop_measurement() 
{
    if (!is_measurement_on()) {
        return false;
    }
    k_timer_stop(&m_heartbeat);
    zbus_chan_notify(&end_measure_chan, ZBUS_TIMEOUT);
    measurement_on = false;
    return measurement_on;
}

void MeasurementState::set_heartbeat(int heartbeat) {
    if (is_measurement_on())
        LOG_WRN("Measurement is ongoing, new heartbeat will be used for next measurement");
    m_heartbeat_duration_ms = heartbeat;
}

// ---- API -------

MeasurementState mst;

bool start_measurement() {
    return mst.start_measurement();
}
bool stop_measurement() {
    return mst.stop_measurement();
}
bool is_measurement_on() {
    return mst.is_measurement_on();
}
void toggle_measurement() {
    mst.toggle_measurement();
}
bool is_measurement_ready() {
    return mst.is_measurement_ready();
}
void toggle_measurement_ready() {
    mst.toggle_measurement_ready();
}
void set_measurement_interval(int heartbeat) {
    mst.set_heartbeat(heartbeat);
}

int get_measurement_interval() {
    return mst.get_heartbeat();
}
