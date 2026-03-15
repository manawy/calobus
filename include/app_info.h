#ifndef CALOBUS_APP_H
#define CALOBUS_APP_H

#include <stdint.h>

/* The information stored in app_info channel of zbus
 *
 *  Generic info about the firmware
 *
 */
struct app_info_msg {
    const char name[32];
    const struct  {
        uint8_t major;
        uint8_t minor;
    } firmware_version;
    const struct {
        uint8_t major;
        uint8_t minor;
    } hardware_version;
};

#endif
