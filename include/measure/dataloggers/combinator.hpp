/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

/* combine the loggers
 *
 * Uses fold expressions (c++17 to iterate over all loggers supplied as types)
 */
template <typename... Loggers>
class CombinedLoggers: public Loggers...
{
public:
    int start(const int64_t& timestamp_0) {
        return (Loggers::start(timestamp_0) + ...);
    }

    int stop() {
        return (Loggers::stop() + ...);
    }

    int log(const struct processing_thread_msg* const data) {
        return (Loggers::log(data) + ...);
    }
};

