/*
 * Calo firmware
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 *
 */

#include "app_info.h"

#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

ZBUS_CHAN_DECLARE(app_info_chan, sensor_data_chan);

ZBUS_OBS_DECLARE(sensor_thread_sub);

ZBUS_CHAN_DEFINE(start_trigger_chan,
                uint8_t,
                NULL, NULL,
                ZBUS_OBSERVERS(sensor_thread_sub),
                0);

void run_trigger() {
    LOG_INF("run trigger");
    zbus_chan_notify(&start_trigger_chan, K_FOREVER);
}

K_TIMER_DEFINE(heartbeat_timer, run_trigger, NULL);

int main() {
    struct app_info_msg* app_info = 
        (struct app_info_msg*) zbus_chan_const_msg(&app_info_chan);


    LOG_INF("%s", app_info->name);
    LOG_INF(" firmware %d.%d", 
            app_info->firmware_version.major,
            app_info->firmware_version.minor);
    LOG_INF(" hardware %d.%d", 
            app_info->hardware_version.major,
            app_info->hardware_version.minor);

    k_timer_start(&heartbeat_timer, K_SECONDS(1), K_SECONDS(1));

    return 0;
}
