#include "util.h"

// Blocking delay! Be carefull using
void delayMs(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 12000; i++) {
        __asm("nop");
    }
}