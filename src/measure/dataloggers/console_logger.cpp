/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 *
 */

#include "measure/dataloggers/console_logger.hpp"
#include "zbus_channels.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(console_logger, CONFIG_LOG_DEFAULT_LEVEL);

int ConsoleDataLogger::start_measurement() {
    LOG_PRINTK("------ Start measurement ------\n");
    return 0;
}

int ConsoleDataLogger::stop_measurement() {
    LOG_PRINTK("------ Stop  measurement ------\n");
    return 0;
}

int ConsoleDataLogger::log_one(const struct processing_thread_msg* const data) {
    LOG_PRINTK("%lld,%d\n",
            (data->timestamp - get_t0())/1000,
            data->value);
    return 0;
}
