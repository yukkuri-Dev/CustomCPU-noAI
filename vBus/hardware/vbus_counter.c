#include "vbus_counter.h"
#include <stdlib.h>

void vbus_counter_init(uint16_t dev_id) {
    vbus_list_add(dev_id, "vBus Counter Device", 1);
}

void vbus_counter_tick(vbus_device_t *dev) {
    if (dev && dev->mem && dev->mem_size >= 1) {
        dev->mem[0] += 1;
    }
}