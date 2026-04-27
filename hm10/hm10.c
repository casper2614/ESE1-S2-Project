// Project includes
#include "hm10.h"
#include "lpuart2.h"
#include "delay.h"

// Global includes
#include <stdio.h>
#include <string.h>

// Function prototype

    /* Test connection to HM-10 and applies necessary settings for iBeacon scanning. Returns integer with error code:
    Code 0 - All good! Initialisation successful
    Code 1 - HM-10 not connected
    Code 2 - HM-10 cannot be set to default settings
    Code 3 - HM-10 cannot switch to master mode
    Code 4 - HM-10 cannot set mode to wait for command before scanning*/
    int hm10_init();

    // Send characters from index 0 to size in buffer to HM-10
    void send(char *buffer, size_t size);

    // Put characters from RX FIFO buffer into buffer of defined size
    void receive(char *buffer, size_t size);

    // Sets characters from index 0 to size to \0 in given buffer
    void wipe(char *buffer, size_t size);

    // Scan for iBeacons and put raw result in buffer of defined size
    void scan(char *buffer, size_t size);

    // Suppress printing of TX and RX to UART-USB
    void suppressPrint(int value);

// Global variables
int suppress = 0;

int hm10_init() {
    suppress = 0;

    char buffer[64];

    printf("Init HM-10...\r\n");
    lpuart2_init(9600);

    send("AT\r\n", 4);
    while (ready == 0) {
        ;;;
    }
    receive(buffer, sizeof(buffer));

    if (strstr(buffer, "OK") == NULL) {
        printf("HM-10 Not connected!\r");
        return 1;
    }
    
    send("AT+RESET\r\n", 10);
    while (ready == 0) {
        ;;;
    }
    receive(buffer, sizeof(buffer));

    if (strstr(buffer, "OK+RESET") == NULL) {
        printf("Can't reset HM-10!\r\n");
        return 2;
    }

    send("AT+ROLE1\r\n", 10);
    while (ready == 0) {
        ;;;
    }
    receive(buffer, sizeof(buffer));

    if (strstr(buffer, "OK+Set:1") == NULL) {
        printf("Can't switch to master mode!\r\n");
        return 3;
    }

    send("AT+IMME1\r\n", 10);
    while (ready == 0) {
        ;;;
    }
    receive(buffer, sizeof(buffer));

    if (strstr(buffer, "OK+Set:1") == NULL) {
        printf("Can't change setting for HM-10 to wait for command to scan!\r\n");
        return 4;
    }

    send("AT+VERR?\r\n", 10);
    while (ready == 0) {
        ;;;
    }
    receive(buffer, sizeof(buffer));

    printf("HM-10 Version: %s\r\n", buffer);

    delayMs(1000);
    printf("HM-10 Init done!\r\n");
    return 0;
}

void send(char *buffer, size_t size) {
    for (int i = 0; i < size; i++) {
        lpuart2_putchar(buffer[i]);
    }

    if (suppress == 0) {
        printf("[TX] - %s\r\n", buffer);
    }
}

void receive(char *buffer, size_t size) {
    ready = 0;
    uint32_t elements = lpuart2_rxcnt();
    // uint8_t eob = 0;
    uint8_t index = 0;

    wipe(buffer, size);

    if (elements > size) {
        printf("There are %d elements in the RX FIFO, the buffer can only fit %d. Data wil be lost!\r\n", (int) elements, (int) size);
    }

    while(lpuart2_rxcnt() > 0 /*&& eob == 0*/) {
        char c = (char) lpuart2_getchar();
        buffer[index] = c;

        
        /* if (c == '\0') {
            eob = 1;
        } */
        

        index++;
    }
    if (suppress == 0) {
        printf("[RX] - %s\r\n", buffer);
    }
}

void wipe(char *buffer, size_t size) {
    memset(buffer, 0, size);
}

void scan(char *buffer, size_t size) {
    wipe(buffer, size);

    send("AT+DISI?\r\n", 10);
    delayMs(1300);
    receive(buffer, size);
}

void suppressPrint(int value) {
    suppress = value;
}