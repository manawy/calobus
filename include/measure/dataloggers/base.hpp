/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

template <class Derived>
class IDatalogger
{
public:
    IDatalogger():
        m_timestamp_0(0)
    {}

    int start(const int64_t& timestamp_0) {
        set_t0(timestamp_0);
        return static_cast<Derived*>(this)->start_measurement();
    }

    void set_t0(const int64_t& timestamp_0) {
        m_timestamp_0 = timestamp_0;
    }

    const int64_t& get_t0() {
        return m_timestamp_0;
    }

    int stop() {
        return static_cast<Derived*>(this)->stop_measurement();
    }

    int log(const struct processing_thread_msg* const data) {
        return static_cast<Derived*>(this)->log_one(data);
    }

private:
    int64_t m_timestamp_0;

};
