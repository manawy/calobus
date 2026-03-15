#include "sensor_thread.h"

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

ZBUS_SUBSCRIBER_DEFINE(sensor_thread_sub, 4);
ZBUS_OBS_DECLARE(processing_thread_sub);

ZBUS_CHAN_DEFINE(sensor_data_chan,
                 struct sensor_data_msg,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(processing_thread_sub),
                 ZBUS_MSG_INIT(0)
                 );

void sensor_thread() {
    const struct zbus_channel* chan;

    struct sensor_data_msg sdata = {.uv = 0};

    int32_t data = 0;

    while(1) {
        zbus_sub_wait(&sensor_thread_sub, &chan, K_FOREVER);

        sdata.uv = ++data;

        zbus_chan_pub(&sensor_data_chan, &sdata, K_MSEC(100));
    }
}

K_THREAD_DEFINE(sensor_thread_id,
                1024,
                sensor_thread,
                NULL, NULL, NULL,
                3, 0,
                0);


