#include "processing_thread.h"

#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(datalogger_thread, LOG_LEVEL_INF);

ZBUS_CHAN_DECLARE(processing_thread_chan);

ZBUS_SUBSCRIBER_DEFINE(datalogger_thread_sub, 4);

ZBUS_CHAN_DEFINE(datalogger_thread_chan,
                 int,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT(0)
                 );

void datalogger_thread() {
    const struct zbus_channel* chan;

    struct processing_thread_msg processed_data;
    int return_code = 0;

    while(1) {
        zbus_sub_wait(&datalogger_thread_sub, &chan, K_FOREVER);


        int err = zbus_chan_read(&processing_thread_chan, &processed_data, K_MSEC(100));
        if (err) {
            LOG_WRN("Could not read processed data channel. Error code: %d", err);
        }
        return_code = err;
        printk("Processed data: %d\n", processed_data.value);

        zbus_chan_pub(&datalogger_thread_chan, &return_code, K_MSEC(100));
    }
}

K_THREAD_DEFINE(datalogger_thread_id,
                1024,
                datalogger_thread,
                NULL, NULL, NULL,
                4, 0,
                0);


