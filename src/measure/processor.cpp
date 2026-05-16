/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include "zbus_channels.h"
#include "measure/sensor.hpp"
#include "zephyr/drivers/sensor.h"

#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(processor_thread, LOG_LEVEL_INF);

class Processor
{
public:
    Processor();

    static constexpr int64_t value_to_fsr(const int64_t& fsr);
    // Return true if gain was changed 
    bool check_gain(const int64_t& value);
    // Return true if gain was changed
    bool set_gain(const int64_t& requested_fsr);
    // Process one
    int process_one(struct sensor_data_msg *msg);
    // Pack the data and send the message
    int pack_and_send();

private:
    void init_current_fsr();

    int64_t m_current_fsr;
    int64_t m_buffer;
    int m_counter;
};

Processor::Processor():
    m_current_fsr(0),
    m_buffer(0),
    m_counter(0)
{
    init_current_fsr();
}

constexpr int64_t Processor::value_to_fsr(const int64_t& value)
{
   int64_t requested_fsr = 0; 
    if (value < 171000) {
        requested_fsr = 256000;
    } else if (value < 426000) {
        requested_fsr = 512000;
    } else if (value < 683000) {
        requested_fsr = 1024000;
    } else {
        requested_fsr = 2048000;
    };
    return requested_fsr;
}

bool Processor::check_gain(const int64_t& value) 
{
    auto requested_fsr = value_to_fsr(value);
    if (requested_fsr == m_current_fsr)
        return false;
    return set_gain(requested_fsr);
}

bool Processor::set_gain(const int64_t& requested_fsr)
{
    struct sensor_attr_msg msg;
    msg.attr = SENSOR_ATTR_GAIN;
    LOG_DBG("Request new gain: %lld", requested_fsr);
    sensor_value_from_micro(&msg.val, requested_fsr);
    int rc = zbus_chan_pub(&sensor_attr_chan, &msg, K_MSEC(100));
    if (rc != 0) 
        return false;
    m_current_fsr = requested_fsr;
    return true;
}

// Process one sample and notify/publish accordingly
int Processor::process_one(struct sensor_data_msg *msg)
{
    if (msg->ok != true) {
        zbus_chan_notify(&end_onebeat_chan, K_MSEC(50));
        return 0;
    }

    int64_t val = sensor_value_to_micro(&msg->uv);
    check_gain(val);

    m_buffer += val;
    if (++m_counter < CONFIG_OVERSAMPLING) {
        // nothing to do - wait for next sample
        zbus_chan_pub(&end_onebeat_chan, &m_counter, K_MSEC(50));
        return 0;
    }
    // data is ready to be send to datalogger
    return pack_and_send();
}

int Processor::pack_and_send()
{
    struct processing_thread_msg processed_data = {.to_save=0, .value=0};
    processed_data.value = m_buffer/CONFIG_OVERSAMPLING;
    processed_data.to_save = true;
    processed_data.timestamp = k_uptime_get();

    // get ready for next loop
    m_buffer = 0;
    m_counter = 0;
    auto rc = zbus_chan_pub(&processing_thread_chan, &processed_data, K_MSEC(50));
    return rc;
}

void Processor::init_current_fsr() {
    auto val = get_sensor_current_fsr();
    m_current_fsr = sensor_value_to_micro(&val);
}


void processing_thread()
{
    const struct zbus_channel* chan;
    struct sensor_data_msg msg;
    Processor processor;

    while(1) {
        zbus_sub_wait(&processing_thread_sub, &chan, K_FOREVER);

        int err = zbus_chan_read(&sensor_data_chan, &msg, K_MSEC(10));
        if (err) {
            LOG_WRN("Could not read data channel. Error code: %d", err);
            continue;;
        }
        processor.process_one(&msg);
    }
}

K_THREAD_DEFINE(processing_thread_id,
                2048,
                processing_thread,
                NULL, NULL, NULL,
                CONFIG_PROCESSING_THREAD_PRIORITY, 0,
                1000);



