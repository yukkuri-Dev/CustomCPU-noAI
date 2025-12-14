#ifndef VBUS_ROOT_H
#define VBUS_ROOT_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint16_t id;
    char name[32];
    uint16_t *mem;
    size_t mem_size;
} vbus_device_t;

typedef struct {
    vbus_device_t *devices;
    size_t capacity;
    size_t count;
} vbus_list_t;

extern vbus_list_t vbus_list;
extern int VBUS_MODE;

void vbus_list_add(uint16_t dev_id, const char *dev_name, size_t mem_size);
int vbus_device_fetch(uint16_t dev_id);
uint16_t vbus_mem_read(vbus_device_t *dev, size_t addr);
void vbus_mem_write(vbus_device_t *dev, size_t addr, uint16_t value);
void vbus_list_init(void);
void vbus_list_print(void);
int vbus_root_main(uint16_t MODE, uint16_t MOUNT_ADDR, uint16_t DATA);
int vbus_root_out_main(uint16_t MODE ,uint16_t MOUNT_ADD,uint16_t DATA);

#endif