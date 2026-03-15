#ifndef CALOBUS_SENSORTHREAD_H
#define CALOBUS_SENSORTHREAD_H

#include <stdint.h>

// The zbus message containing the raw data
struct sensor_data_msg {
    int32_t uv;
};

#endif // CALOBUS_SENSORTHREAD_H
