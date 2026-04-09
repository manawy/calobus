# Copyright (c) 2026 F. Georget
#  SPDX-License-Identifier: Apache-2.0


add_subdirectory_ifdef(CONFIG_DT_HAS_ZEPHYR_EMUL_FLUXSENSOR_ENABLED fluxsensor)
add_subdirectory_ifdef(CONFIG_DT_HAS_ZEPHYR_EMUL_TEMPSENSOR_ENABLED tempsensor)
