/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/shell/shell.h>
#include "measure/toggle.h"

static int start_handler(const struct shell *sh, size_t argc,
                        char **argv) {
    start_measurement();
    return 0;
}
static int end_handler(const struct shell *sh, size_t argc,
                        char **argv) {
    end_measurement();
    return 0;
}
static int status_handler(const struct shell *sh, size_t argc,
                        char **argv) {

    if (is_measurement_on()) {
        shell_print(sh, "Measurement ongoing");
    } else {
        shell_print(sh, "No measurement ongoing");
    }
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_measure,
        SHELL_CMD(start, NULL, "Start measurement.", start_handler),
        SHELL_CMD(stop,   NULL, "Stop measurement.", end_handler),
        SHELL_CMD(status, NULL, "Status of the measurement.", status_handler),
        SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(measure, &sub_measure, "Measure control command", NULL);
