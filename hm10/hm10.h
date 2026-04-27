#ifndef HM10_H
#define HM10_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    char uuid[16];
    uint16_t major;
    uint16_t minor;
    int8_t transmit;
    int8_t RSSI;
} iBeacon_t;

int hm10_init();
void send(char *buffer, size_t size);
void receive(char *buffer, size_t size);
void wipe(char *buffer, size_t size);
void scan(char *buffer, size_t size);
void suppressPrint(int value);


#endif