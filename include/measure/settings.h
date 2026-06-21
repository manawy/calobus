/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum measure_setting {
    SETTING_NONE,
    SETTING_OVERSAMPLING,
};

struct measure_setting_msg {
    enum measure_setting setting;
    int value;
};

#ifdef __cplusplus
}
#endif
