/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 *
 */

#pragma once

#include "measure/dataloggers/base.hpp"

/* The console logger
 *
 * Print each measurement to the console
 */
class ConsoleDataLogger: public IDatalogger<ConsoleDataLogger>
{
public:
    ConsoleDataLogger() = default;
    int start_measurement();
    int stop_measurement();
    int log_one(const struct processing_thread_msg* const data);
};
