#ifndef VBUS_COUNTER_H
#define VBUS_COUNTER_H

#include "../vbus_root.h"

void vbus_counter_init(uint16_t dev_id);
void vbus_counter_tick(vbus_device_t *dev);

#endif

