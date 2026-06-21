/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/shell/shell.h>
#include "measure/toggle.h"
#include <zephyr/drivers/uart.h>
#include "measure/settings.h"
#include "zbus_channels.h"
#include "zephyr/sys/clock.h"

#include <stdlib.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(shell_cmds, CONFIG_LOG_DEFAULT_LEVEL);


//    Console
// -------------
//
void console_init() {
    #ifdef CONFIG_WAIT_CONSOLE_INIT
    #if DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart)
        const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
        uint32_t dtr = 0;

        /* Poll if the DTR flag was set */
        while (!dtr) {
            uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
            /* Give CPU resources to low priority threads. */
            k_sleep(K_MSEC(100));
        }
    #endif
    #endif
}


// Shell
// -----

static int start_handler(const struct shell *sh, size_t argc,
                        char **argv) {
    start_measurement();
    return 0;
}
static int end_handler(const struct shell *sh, size_t argc,
                        char **argv) {
    stop_measurement();
    return 0;
}
static int status_handler(const struct shell *sh, size_t argc,
                        char **argv) {

    if (is_measurement_on()) {
        shell_print(sh, "Measurement ongoing");
    } else {
        shell_print(sh, "No measurement ongoing");
        if (is_measurement_ready()) 
            shell_print(sh, "Measurement ready");
        else
            shell_print(sh, "Measurement not ready");
    }
    return 0;
}

static int sh_set_oversampling(const struct shell *sh, size_t argc,
                            char **argv) {
    struct measure_setting_msg msg;
    msg.value = atoi(argv[argc-1]);
    if (msg.value <= 0) {
        LOG_ERR("Invalid value %s.", argv[argc-1]);
        return EINVAL;
    }
    msg.setting = SETTING_OVERSAMPLING;
    zbus_chan_pub(&measure_setting_chan, &msg, K_MSEC(50));
    return 0;
}

static int sh_get_oversampling(const struct shell *sh, size_t argc,
                            char **argv) {
    struct measure_setting_msg msg;
    msg.value = -1; // -1 is magic value to request
    msg.setting = SETTING_OVERSAMPLING;
    zbus_chan_pub(&measure_setting_chan, &msg, K_MSEC(50));
    return 0;
}


static int sh_set_measurement_interval(const struct shell *sh, size_t argc,
                            char **argv) {
    int value = atoi(argv[argc-1]);
    if (value <= 50) {
        LOG_ERR("Invalid value %s.", argv[argc-1]);
        return EINVAL;
    }
    set_measurement_interval(value);
    return 0;
}

static int sh_get_measurement_interval(const struct shell *sh, size_t argc,
                            char **argv) {
    int val = get_measurement_interval();
    LOG_PRINTK("Measurement interval: %i ms.\n", val);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_measure_setting_set,
        SHELL_CMD_ARG(interval, NULL, "Set the measurement interval", sh_set_measurement_interval, 2, 0),
        SHELL_CMD_ARG(oversampling, NULL, "Set the oversampling", sh_set_oversampling, 2, 0),
        SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_measure_setting_get,
        SHELL_CMD_ARG(interval, NULL, "Get the measurement interval", sh_get_measurement_interval, 1, 0),
        SHELL_CMD_ARG(oversampling, NULL, "Get the oversampling", sh_get_oversampling, 1, 0),
        SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_measure_setting,
        SHELL_CMD(set, &sub_measure_setting_set, "Set setting", NULL),
        SHELL_CMD(get, &sub_measure_setting_get, "Get current setting", NULL),
        SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_measure,
        SHELL_CMD(start, NULL, "Start measurement", start_handler),
        SHELL_CMD(stop,   NULL, "Stop measurement", end_handler),
        SHELL_CMD(status, NULL, "Status of the measurement", status_handler),
        SHELL_CMD(setting, &sub_measure_setting, "Set a parameter", NULL),
        SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(measure, &sub_measure, "Measure control command", NULL);
