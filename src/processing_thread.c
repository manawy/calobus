/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include "processing_thread.h"
#include "sensor_thread.h"
#include "zephyr/drivers/sensor.h"

#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(core_thread, LOG_LEVEL_INF);


ZBUS_SUBSCRIBER_DEFINE(processing_thread_sub, 4);
ZBUS_OBS_DECLARE(datalogger_thread_sub);

ZBUS_CHAN_DECLARE(end_onebeat_chan) ;
ZBUS_CHAN_DEFINE(processing_thread_chan,
                 struct processing_thread_msg,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(datalogger_thread_sub),
                 ZBUS_MSG_INIT(0)
                 );

// Process one sample and notify/publish accordingly
static void process_one(struct sensor_data_msg *msg)
{
    static int64_t buffer = 0;
    static int counter = 0;

    if (msg->ok != true) {
        zbus_chan_notify(&end_onebeat_chan, K_MSEC(50));
    }

    struct processing_thread_msg processed_data = {.to_save=0, .value=0};
    buffer += msg->uv.val1;

    if (++counter < CONFIG_OVERSAMPLING) {
        // nothing to do - wait for nex sample
        zbus_chan_pub(&end_onebeat_chan, &counter, K_MSEC(50));
        return;
    }

    // data is ready to be send to datalogger
    processed_data.value = buffer/CONFIG_OVERSAMPLING;
    processed_data.to_save = msg->ok;
    processed_data.timestamp = k_uptime_get();

    buffer = 0;
    counter = 0;
    zbus_chan_pub(&processing_thread_chan, &processed_data, K_MSEC(50));
    return;
}

void processing_thread()
{
    const struct zbus_channel* chan;
    struct sensor_data_msg msg;

    while(1) {
        zbus_sub_wait(&processing_thread_sub, &chan, K_FOREVER);

        int err = zbus_chan_read(&sensor_data_chan, &msg, K_MSEC(10));
        if (err) {
            LOG_WRN("Could not read data channel. Error code: %d", err);
            continue;;
        }
        process_one(&msg);
    }
}

K_THREAD_DEFINE(processing_thread_id,
                2048,
                processing_thread,
                NULL, NULL, NULL,
                CONFIG_PROCESSING_THREAD_PRIORITY, 0,
                1000);



