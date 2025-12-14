#ifndef FAKEVGQ_H
#define FAKEVGQ_H

#include <stdint.h>
#include "../vbus_root.h"

void fakevgq_init(uint16_t dev_id);
void fakevgq_tick(vbus_device_t *dev);

#endif
