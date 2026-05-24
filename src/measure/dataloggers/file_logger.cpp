/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 *
 */

#include "measure/dataloggers/file_logger.hpp"
#include "calo_time.h"
#include "zbus_channels.h"
#include "filesystem.h"

#include <zephyr/logging/log.h>
#include <zephyr/sys/timeutil.h>

LOG_MODULE_REGISTER(filedatalogger, CONFIG_LOG_DEFAULT_LEVEL);

FileDataLogger::~FileDataLogger()
{
    close_file();
}

int FileDataLogger::start_measurement()
{
    if (!open_file()) 
        return -EIO;
    write_header();
    return 0;
}

void FileDataLogger::write_header()
{
    constexpr char buf[] = "# time (s), voltage (uV)\n";
    fs_write(&m_file, buf, strlen(buf));
}

int FileDataLogger::stop_measurement()
{
    if (close_file())
        return 0;
    else
        return -EIO;
}

bool FileDataLogger::open_file()
{

    if (m_file_open) return m_file_open;

    char buf[128];
    struct tm tm;
    get_time(&tm);

    fs_file_t_init(&m_file);
    get_sd_timed_path(buf, "m", &tm, ".dat");

    int rc = fs_open(&m_file, buf, FS_O_CREATE | FS_O_RDWR | FS_O_APPEND); 
    if (rc != 0) {
        LOG_ERR("Failed to open file %s", buf);
    } else {
       m_file_open = true;
    }

    return m_file_open;
}

bool FileDataLogger::close_file()
{
    if (m_file_open)
    {
        fs_close(&m_file);
        m_file_open = false;
    }
    return m_file_open;
}

int FileDataLogger::log_one(const struct processing_thread_msg* const data)
{
    if (!m_file_open)
        return -EIO;

    snprintf(m_buf, 128, "%lld,%d\n",
        (data->timestamp - get_t0())/1000,
        data->value);
    int ret = fs_write(&m_file, &m_buf, strlen(m_buf));

    if (ret <0)
    {
        LOG_ERR("failed to write to file");
        return -EIO;
    }

    if (++m_cache_count >= CONFIG_NB_WRITE_FLUSH) {
        fs_sync(&m_file);
        m_cache_count = 0;
    }

    return 0;
}
