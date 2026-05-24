/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 *
 */

#pragma once

#include "measure/dataloggers/base.hpp"
#include <zephyr/fs/fs.h>

/* The file logger
 *
 * Save each measurement in a data file
 */
class FileDataLogger: public IDatalogger<FileDataLogger>
{
public:
    FileDataLogger():
        m_file_open(false),
        m_cache_count(0)
    {}

    ~FileDataLogger();

    int start_measurement();

    int stop_measurement();

    int log_one(const struct processing_thread_msg* const data);

private:
    bool open_file();
    bool close_file();
    void write_header();

    bool m_file_open;
    struct fs_file_t m_file;
    char m_buf[128];
    int m_cache_count;
};
