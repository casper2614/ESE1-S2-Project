#include <board.h>
#include <stdio.h>

#include "leds.h"
#include "serial.h"
#include "hm10.h"

// -----------------------------------------------------------------------------
// Local type definitions
// -----------------------------------------------------------------------------
#ifdef DEBUG
#define TARGETSTR "Debug"
#else
#define TARGETSTR "Release"
#endif

// -----------------------------------------------------------------------------
// Local function prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Main application
// -----------------------------------------------------------------------------
int main(void)
{
    leds_init();
    serial_init(115200);

    printf("iBeacon Scanning\r\n");
    printf("%s build %s %s\r\n", TARGETSTR, __DATE__, __TIME__);

    int result = hm10_init();
    suppressPrint(1);

    if (result != 0) {
        led_red_on();
        while (1) {;;;}
    } else {
        led_green_on();
    }

    while (1) {
        char buffer[2048];
        scan(buffer, sizeof(buffer));

        printf("Scanning result:\r\n%s", buffer);
    }
     

}

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------
