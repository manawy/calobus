/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/syscalls/kernel.h>
#include <zephyr/sys/__assert.h>
#include <stdint.h>
#define DT_DRV_COMPAT zephyr_emul_tempsensor


#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "tempsensor.h"


static int tempsensor_sample_fetch(
    const struct device *dev,
    enum sensor_channel chan)
{
    __ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL || 
                    chan == SENSOR_CHAN_AMBIENT_TEMP);

    k_busy_wait(1);
    k_sleep(K_MSEC(10)); // according datasheet
    k_busy_wait(1);
    return 0;
}

static int tempsensor_channel_get(
    const struct device *dev,
    enum sensor_channel chan,
    struct sensor_value *val)
{

    __ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL || 
                    chan == SENSOR_CHAN_AMBIENT_TEMP);

    sensor_value_from_float(val, 22.5);
    return 0;
}



static DEVICE_API(sensor, tempsensor_api_funcs) = {
    .sample_fetch = tempsensor_sample_fetch,
    .channel_get = tempsensor_channel_get,
};

int tempsensor_init(const struct device *dev) {

    struct tempsensor_data *dev_data = dev->data;
    //const struct tempsensor_config *dev_cfg = dev->config;

    dev_data->fsr_micro = 512000;
    return 0;
}

#define TEMPSENSOR_DEFINE(inst)                                                 \
    static struct tempsensor_data tempsensor_data_##inst;                       \
                                                                                \
    static const struct tempsensor_config tempsensor_config_##inst = {};        \
                                                                                \
    SENSOR_DEVICE_DT_INST_DEFINE(inst,                                          \
                                 tempsensor_init,                               \
                                 NULL,                                          \
                                 &tempsensor_data_##inst,                       \
                                 &tempsensor_config_##inst,                     \
                                 POST_KERNEL,                                   \
                                 CONFIG_SENSOR_INIT_PRIORITY,                   \
                                 &tempsensor_api_funcs                          \
                             );

DT_INST_FOREACH_STATUS_OKAY(TEMPSENSOR_DEFINE)

