#include "hm10.h"
#include "lpuart2.h"
#include "util.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

static char buffer[BUFFER_SIZE];
char commands[4][64] = {"AT+RESET\r\n", "AT+ROLE1\r\n", "AT+IMME1\r\n", "AT+VERR?\r\n"};

typedef struct {
    char[34] UUID,
    int major,
    int minor,
    

} beacon_t

int init(void) {
    send("AT");
    delayMs(500);
    receive();

    if (strcmp(buffer, "OK\0\n") != 0) {
        return -1; // HM10 Not connected!
    }

    for (int i = 0; i < 4; i++) {
        send(commands[i]);
        delayMs(500);
        receive();
    }

    printf("HM10 Initialisation done!\r\n");

    return 0;
}

void receive(void) {
    wipe(); // Clear buffer
    uint16_t index = 0;
    bool eob = false;
    uint32_t uartelements = lpuart2_rxcnt(); // How many bytes are currently being stored in the UART buffer?
    

    /*
    Will the UART2 receive buffer data fit inside memory?
    0 - the buffer is empty
    512 - 511 bytes with \0, fits perfectly
    >>513 - To big for the buffer, unless the buffer size is changed.
    */  
    if (uartelements > BUFFER_SIZE) { 
        printf("Warning! The data in the UART2 receive buffer is to big to fit inside memory!\r\n");
    }

    while (lpuart2_rxcnt() > 0 || eob == false) {
        char c = (char) lpuart2_getchar(); // Get character 
        buffer[index] = c; // Insert into buffer

        if (c == '\0') {
            eob = true;
        }
        index++;
    }

    printf("Received: %s\r\n", buffer);
}

void send(char data[]) {
    for (uint16_t i = 0; data[i] != '\0'; i++)
    {
        lpuart2_putchar((uint8_t)data[i]);
    }
    
    printf("Sending: %s\r\n", data); // Print to console
}

void wipe(void) {
    memset(&buffer, 0, BUFFER_SIZE);
}

void scan() {
    send("AT+DISI?\r\n");
    delayMs(1000);
    receive();


}