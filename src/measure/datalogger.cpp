/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 *
 * datalogger_thread
 * -----------------
 *
 *  The thread responsible for datalogging
 *
 *  Datalogging occurs on:
 *      On LOG_PRINTK output
 *      SD card: /SD:/m<date><time>.dat if CONFIG_SDLOGGING
 *
 * This thread is responsible for handling file creation, opening and closing
 */

#include "measure/processor.h"
#include "measure/toggle.h"
#include "zbus_channels.h"
#include "calo_time.h"

#include <stdint.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/rtc.h>

#ifdef CONFIG_SDLOGGING
#include "filesystem.h"
#include <zephyr/fs/fs.h>
#endif

LOG_MODULE_REGISTER(datalogger_thread, LOG_LEVEL_INF);

class FileDatalogger
{
public:
    FileDatalogger():
        m_file_open(false),
        m_timestamp_0(0)
    {}

    ~FileDatalogger();

    int start_measurement();

    int end_measurement();

    int log_one(const struct processing_thread_msg* const data);

private:
    bool open_file();
    bool close_file();
    void write_header();

    bool m_file_open;
    int64_t m_timestamp_0;
    struct fs_file_t m_file;
    char m_buf[128];
};


FileDatalogger::~FileDatalogger() 
{
    close_file();
}

int FileDatalogger::start_measurement()
{
    m_timestamp_0 = k_uptime_get();

    if (open_file()) {
        write_header();
        return 0;
    }
    else {
        return -EIO;
    }
}

void FileDatalogger::write_header() 
{
    constexpr char buf[] = "# timestamp, voltage";
    fs_write(&m_file, buf, strlen(buf));
}

int FileDatalogger::end_measurement()
{
    if (close_file())
        return 0;
    else
        return -EIO;
}


bool FileDatalogger::open_file() 
{

    if (m_file_open) return m_file_open;

    struct tm tm;
    get_time(&tm);

    fs_file_t_init(&m_file);
    get_sd_timed_path(m_buf, "m", &tm, ".dat");

    int rc = fs_open(&m_file, m_buf, FS_O_CREATE | FS_O_RDWR | FS_O_APPEND); 
    if (rc != 0) {
        LOG_ERR("Failed to open file %s", m_buf);
    } else {
       m_file_open = true;
    }

    return m_file_open;
}

bool FileDatalogger::close_file() 
{
    if (m_file_open)
    {
        fs_close(&m_file);
        m_file_open = false;
    }
    return m_file_open;
}

int FileDatalogger::log_one(const struct processing_thread_msg* const data)
{
    if (!m_file_open)
        return -EIO;

    sprintf(m_buf, "%lld,%d\n",
            data->timestamp - m_timestamp_0,
            data->value);
    int ret = fs_write(&m_file, m_buf, strlen(m_buf));
    if (ret <0)
        return -EIO;

    return 0;

}

// ---- Static global variables ------------------


// Time at the start of the measurement
static int64_t timestamp_0;

// ---- helper functions --------------------------



// Log one data-point
static int datalog_one(const struct processing_thread_msg* const processed_data) 
{
    LOG_PRINTK("%lld,%d\n", processed_data->timestamp - timestamp_0, processed_data->value);
    return 0;
}

// Setup the datlogging at the start of the measurement
static int datalog_setup() 
{
    struct tm tm;

    int ret = get_time(&tm);
    timestamp_0 = k_uptime_get();
    if (ret <0) {
        LOG_ERR("Cannot get RTC time ! return code : %d", ret);
    }

    LOG_PRINTK("#Initial timestamp: ");
    LOG_PRINTK("%02d-%02d-%02d %02d:%02d:%02d ",
                tm.tm_year-100,
                tm.tm_mon+1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec);
    LOG_PRINTK("%lld\n", timestamp_0);

    return 0;
}

// Clean the datalogging environment at the end of the measurement
static int datalog_end() {
    LOG_DBG("Datalogging end");
    return 0;
}

// ----- Thread ----------------------------------------

void datalogger_thread(void) {
    const struct zbus_channel* chan;
    struct processing_thread_msg processed_data;
    FileDatalogger fdatalog;

    while(1) {
        zbus_sub_wait(&datalogger_thread_sub, &chan, K_FOREVER);

        if (&processing_thread_chan == chan){
            zbus_chan_read(&processing_thread_chan,
                             &processed_data, K_MSEC(50));
            int rc = datalog_one(&processed_data);
            fdatalog.log_one(&processed_data);
            zbus_chan_pub(&end_onebeat_chan, &rc, K_MSEC(50));
        } else if (&start_measure_chan == chan) {
            fdatalog.start_measurement();
            datalog_setup();
        } else if (&end_measure_chan == chan) {
            fdatalog.end_measurement();
            datalog_end();
        }
    }
}

K_THREAD_DEFINE(datalogger_thread_id,
                2048,
                datalogger_thread,
                NULL, NULL, NULL,
                CONFIG_DATALOGGER_THREAD_PRIORITY, 0,
                1000);


