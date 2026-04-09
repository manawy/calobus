/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/syscalls/kernel.h>
#include <zephyr/sys/__assert.h>
#include <stdint.h>
#define DT_DRV_COMPAT zephyr_emul_fluxsensor


#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "fluxsensor.h"


static int fluxsensor_sample_fetch(
    const struct device *dev,
    enum sensor_channel chan)
{
    __ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL || 
                    chan == SENSOR_CHAN_VOLTAGE)

    k_busy_wait(1);
    k_sleep(K_MSEC(10)); // according datasheet
    k_busy_wait(1);
    return 0;
}

static int fluxsensor_channel_get(
    const struct device *dev,
    enum sensor_channel chan,
    struct sensor_value *val)
{

    int64_t tmp;

    __ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL || 
                    chan == SENSOR_CHAN_VOLTAGE)

    tmp = 5 * 512000;
    sensor_value_from_micro(val, tmp/36768);

    return 0;
}


static int fluxsensor_attr_get(
    const struct device *dev,
    enum sensor_channel chan,
    enum sensor_attribute attr,
    struct sensor_value *val)
{
    __ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL ||
                    chan == SENSOR_CHAN_VOLTAGE)

    struct fluxsensor_data *dev_data = dev->data;

    switch (attr) {
    case SENSOR_ATTR_GAIN:
        sensor_value_from_micro(val, dev_data->fsr_micro);
        break;
    default:
        return -EINVAL;
    }
    return 0;
}

int set_gain(struct fluxsensor_data *dev_data, int fsr_micro)
{
    switch (fsr_micro) {
        case 256000:
            dev_data->fsr_micro = fsr_micro;
            break;
        case 512000:
            dev_data->fsr_micro = fsr_micro;
            break;
        case 1024000:
            dev_data->fsr_micro = fsr_micro;
            break;
        case 2048000:
            dev_data->fsr_micro = fsr_micro;
            break;
        case 4096000:
            dev_data->fsr_micro = fsr_micro;
            break;
        case 6144000:
            dev_data->fsr_micro = fsr_micro;
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static int fluxsensor_attr_set(
    const struct device *dev,
    enum sensor_channel chan,
    enum sensor_attribute attr,
    const struct sensor_value *val)
{

    __ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL ||
                    chan == SENSOR_CHAN_VOLTAGE)

    struct fluxsensor_data *dev_data = dev->data;

    switch (attr) {
    case SENSOR_ATTR_GAIN:
        return set_gain(dev_data, sensor_value_to_micro(val));
        break;
    default:
        return -EINVAL;
    }
    return 0;
}

static DEVICE_API(sensor, fluxsensor_api_funcs) = {
    .sample_fetch = fluxsensor_sample_fetch,
    .channel_get = fluxsensor_channel_get,
    .attr_get = fluxsensor_attr_get,
    .attr_set = fluxsensor_attr_set,
};

int fluxsensor_init(const struct device *dev) {

    struct fluxsensor_data *dev_data = dev->data;
    const struct fluxsensor_config *dev_cfg = dev->config;

    dev_data->fsr_micro = 512000;
    return 0;
}

#define FLUXSENSOR_DEFINE(inst)                                                 \
    static struct fluxsensor_data fluxsensor_data_##inst;                       \
                                                                                \
    static const struct fluxsensor_config fluxsensor_config_##inst {}           \
                                                                                \
    SENSOR_DEVICE_DT_INST_DEFINE(inst,                                          \
                                 fluxsensor_init,                               \
                                 NULL,                                          \
                                 &fluxsensor_data_##inst,                       \
                                 &fluxsensor_config_##inst,                     \
                                 POST_KERNEL,                                   \
                                 CONFIG_SENSOR_INIT_PRIORITY,                   \
                                 &fluxsensor_api_funcs                          \
                             );

DT_INST_FOREACH_STATUS_OKAY(FLUXSENSOR_DEFINE)

