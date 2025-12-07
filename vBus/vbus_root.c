#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
int VBUS_MODE = 0;
#define VBUS_INIT_CAPACITY 8

typedef struct {
    uint16_t id;
    char name[32]; // デバイス名
} vbus_device_t;

typedef struct {
    vbus_device_t *devices;
    size_t capacity;
    size_t count;
} vbus_list_t;

vbus_list_t vbus_list;

void vbus_list_init() {
    vbus_list.devices = (vbus_device_t *)malloc(sizeof(vbus_device_t) * VBUS_INIT_CAPACITY);
    vbus_list.capacity = VBUS_INIT_CAPACITY;
    vbus_list.count = 0;
    vbus_list_add(0x0000, "CloverTechno Inc. vBus Root Ctroler");
    vbus_list_add(0x0001, "FRoJ Fdn. vBus Network Adapter");// Federative Republic of Japan Open Source Foundation Bus Network Adapter
    vbus_list_add(0x0002, "Hoffnung Inc. vBus Display Adapter");
    vbus_list_add(0x0003, "JohnTech vBus Storage Controller");//JohnTech vBus Storage Controller
    vbus_list_add(0x0004, "FRoJ Fdn. vBus Input Device Ctroler");
    vbus_list_add(0x0010, "Generic vBus Keyboard Device");
    vbus_list_add(0x0005, "CloverTechno Inc. vBus Audio Device");
}

void vbus_list_add(uint16_t dev_id, const char *dev_name) {
    if (vbus_list.count >= vbus_list.capacity) {
        vbus_list.capacity *= 2;
        vbus_list.devices = (vbus_device_t *)realloc(vbus_list.devices, sizeof(vbus_device_t) * vbus_list.capacity);
    }
    vbus_list.devices[vbus_list.count].id = dev_id;
    strncpy(vbus_list.devices[vbus_list.count].name, dev_name, 31);
    vbus_list.devices[vbus_list.count].name[31] = '\0';
    vbus_list.count++;
}

void vbus_list_print() {
    for (size_t i = 0; i < vbus_list.count; i++) {
        printf("Device #%zu: ID=%04X, Name=%s\n", i, vbus_list.devices[i].id, vbus_list.devices[i].name);
    }
}
int vbus_root_main(uint16_t MODE, uint16_t ADDR, uint16_t DATA) {
    if (MODE == 0xFFFF){//INIT
        vbus_list_init();
        VBUS_MODE = 1;//Initialized
        return 100;
    }else if(MODE == 0x0000){//READ
            vbus_list_print();
    }
    if (VBUS_MODE == 0) {
        return -1;
    }
    
    printf("[vBus] vBus Root Controller Initialized.\n");
}