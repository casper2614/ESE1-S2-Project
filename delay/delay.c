#include "delay.h"

// Function prototypes

    // Blocking delay that (approximately) waits for given time in milliseconds
    void delayMs(uint32_t ms);

void delayMs(uint32_t ms) {
    for (volatile int i = 0; i < ms * 12000; i++) {
        __asm("nop");
    }
}