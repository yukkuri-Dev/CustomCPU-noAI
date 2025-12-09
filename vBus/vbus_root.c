#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
int VBUS_MODE = 0;
#define VBUS_INIT_CAPACITY 8

typedef struct {
    uint16_t id;
    char name[32]; // デバイス名
    uint16_t *mem;
    size_t mem_size;
} vbus_device_t;

typedef struct {
    vbus_device_t *devices;
    size_t capacity;
    size_t count;


} vbus_list_t;

vbus_list_t vbus_list;

void vbus_mem_write(vbus_device_t *dev, size_t addr, uint16_t value) {
    if (addr < dev->mem_size) {
        dev->mem[addr] = value;
    } else {
        printf("[vBus] Write: Out of range! addr=%zu\n", addr);
    }
}
uint16_t vbus_mem_read(vbus_device_t *dev, size_t addr) {
    if (addr < dev->mem_size) {
        return dev->mem[addr];
    } else {
        printf("[vBus] Read: Out of range! addr=%zu\n", addr);
        return 0xFFFF;
    }
}

void vbus_list_init() {
    vbus_list.devices = malloc(sizeof(vbus_device_t) * VBUS_INIT_CAPACITY);
    if (vbus_list.devices == NULL) {
        printf("[critical]Failed to initialize vBus device list memory.\n");
        exit(1);
    }
    vbus_list.capacity = VBUS_INIT_CAPACITY;
    vbus_list.count = 0;
    vbus_list_add(0x0000, "vPU vBus Root Controller", 0);    // メモリ不要デバイス
    vbus_list_add(0x0001, "vPU vBus Example Device 1", 128); // 128 wordsのメモリ付き
    vbus_list_add(0x0002, "CloverTech Graphics Server", 256); // 256 wordsのメモリ付き
    for (int i = 0; i < 128; i++) {
        // 本来はこんな形で書く：
        for (int i = 0; i < 128; ++i) {
            uint16_t raw[] = {0x0001, 0x0002, 0xC0DE, 0xFACE};
            vbus_mem_write(&vbus_list.devices[2], i, raw[i % 4]); // raw[0~3]を繰り返す
        }
    }
    vbus_mem_write(&vbus_list.devices[2], 0, 0x1234); // VGAのメモリに値を書き込む
    }

void vbus_list_add(uint16_t dev_id, const char *dev_name, size_t mem_size) {
    if (vbus_list.count >= vbus_list.capacity) {
        vbus_list.capacity *= 2;
        vbus_list.devices = realloc(vbus_list.devices, sizeof(vbus_device_t) * vbus_list.capacity);
        if (!vbus_list.devices) {
            fprintf(stderr, "[vBus] realloc error\n");
            exit(1);
        }
    }
    vbus_device_t *dev = &vbus_list.devices[vbus_list.count++];
    dev->id = dev_id;
    strncpy(dev->name, dev_name, sizeof(dev->name)-1);
    dev->name[sizeof(dev->name)-1] = '\0';
    if (mem_size > 0) {
        dev->mem = malloc(sizeof(uint16_t) * mem_size);
        if (!dev->mem) {
            fprintf(stderr, "[vBus] device mem allocation error\n");
            exit(1);
        }
        dev->mem_size = mem_size;
        memset(dev->mem, 0, sizeof(uint16_t) * mem_size);
    } else {
        dev->mem = NULL;
        dev->mem_size = 0;
    }
}
int vbus_device_fetch(uint16_t dev_id) {
    for (size_t i = 0; i < vbus_list.count; i++) {
        if (vbus_list.devices[i].id == dev_id) {
            return i; // デバイスが見つかった場合、そのインデックスを返す
        }
    }
    return -1; // デバイスが見つからなかった場合
}
void vbus_list_print() {
    for (size_t i = 0; i < vbus_list.count; i++) {
        printf("Device #%zu: ID=%04X, Name=%s\n", i, vbus_list.devices[i].id, vbus_list.devices[i].name);
    }
}
int vbus_root_main(uint16_t MODE, uint16_t MOUNT_ADDR, uint16_t DATA) {
    if (MODE == 0xFFFF){//INIT
        vbus_list_init();
        VBUS_MODE = 1;//Initialized
        return -100;
    }else if(MODE == 0x0000){//READ
        int idx = vbus_device_fetch(MOUNT_ADDR);
        if (idx == -1) {
            printf("[vBus] Read Error: Device ID %04X not found.\n", MOUNT_ADDR);
            return -10;
        }else{
            DATA = vbus_mem_read(&vbus_list.devices[idx], DATA);
            printf("[vBus] Read from Device ID %04X: Data=%04X\n", MOUNT_ADDR, DATA);
            return DATA;
        }

    }
    if (VBUS_MODE == 0) {
        return -1;
    }
    
    printf("[vBus] vBus Root Controller Initialized.\n");
}
int vbus_root_out_main(uint16_t MODE ,uint16_t MOUNT_ADD,uint16_t DATA) {

    
}