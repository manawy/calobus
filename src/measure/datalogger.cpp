/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 *
 * datalogger_thread
 * -----------------
 *
 *  The thread responsible for datalogging
 *
 *  Datalogging occurs on:
 *      On LOG_PRINTK output
 *      SD card: /SD:/m<date><time>.dat if CONFIG_SDLOGGING
 *
 * This thread is responsible for handling file creation, opening and closing
 */

#include "zbus_channels.h"
#include "zephyr/zbus/zbus.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "measure/dataloggers/combinator.hpp"
#include "measure/dataloggers/console_logger.hpp"
#ifdef CONFIG_SDLOGGING
#include "measure/dataloggers/file_logger.hpp"
#endif

#ifdef CONFIG_SDLOGGING
static CombinedLoggers<FileDataLogger, ConsoleDataLogger> Logger;
#else
static CombinedLoggers<ConsoleDataLogger> Logger;
#endif

void datalogger_thread(void) {
    const struct zbus_channel* chan;

    while(1) {
        zbus_sub_wait(&datalogger_thread_sub, &chan, K_FOREVER);

        if (&processing_thread_chan == chan){
            struct processing_thread_msg processed_data;
            zbus_chan_read(&processing_thread_chan,
                             &processed_data, K_MSEC(50));
            int rc = Logger.log(&processed_data);
            zbus_chan_pub(&end_onebeat_chan, &rc, K_MSEC(50));
        } else if (&start_measure_chan == chan) {
            auto timestamp_0 = k_uptime_get();
            Logger.start(timestamp_0);
        } else if (&end_measure_chan == chan) {
            Logger.stop();
            zbus_chan_notify(&measurement_ready_chan, K_MSEC(50));
        }
    }
}

K_THREAD_DEFINE(datalogger_thread_id,
                2048,
                datalogger_thread,
                NULL, NULL, NULL,
                CONFIG_DATALOGGER_THREAD_PRIORITY, 0,
                1000);


