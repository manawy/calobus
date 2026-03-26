#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <zephyr/drivers/sensor.h>

// The zbus message containing the raw data
struct sensor_data_msg {
    struct sensor_value uv;
    struct sensor_value temp;
    bool ok;
};

