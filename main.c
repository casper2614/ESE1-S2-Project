/*! ***************************************************************************
 *
 * \brief     Main application
 * \file      main.c
 * \author    Hugo Arends
 * \date      February 2024
 *
 * \see       NXP. (2024). MCX A153, A152, A143, A142 Reference Manual. Rev. 4,
 *            01/2024. From:
 *            https://www.nxp.com/docs/en/reference-manual/MCXAP64M96FS3RM.pdf
 *
 * \copyright 2024 HAN University of Applied Sciences. All Rights Reserved.
 *            \n\n
 *            Permission is hereby granted, free of charge, to any person
 *            obtaining a copy of this software and associated documentation
 *            files (the "Software"), to deal in the Software without
 *            restriction, including without limitation the rights to use,
 *            copy, modify, merge, publish, distribute, sublicense, and/or sell
 *            copies of the Software, and to permit persons to whom the
 *            Software is furnished to do so, subject to the following
 *            conditions:
 *            \n\n
 *            The above copyright notice and this permission notice shall be
 *            included in all copies or substantial portions of the Software.
 *            \n\n
 *            THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *            EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *            OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *            NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *            HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *            WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *            FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *            OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#include <board.h>
#include <stdio.h>

#include "leds.h"
#include "switches.h"
#include "serial.h"

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
    sw_init();
    serial_init(115200);

    printf("Template example\r\n");
    printf("%s build %s %s\r\n", TARGETSTR, __DATE__, __TIME__);

    printf("1. Verify that after microcontroller reset a message appears\r\n" \
        "   in the terminal application.\r\n" \
        "2. Verify that typing the characters 'r', 'g' and 'b' in the \r\n" \
        "   terminal application toggles the corresponding RBG LED.\r\n" \
        "3. Verify that pressing SW2 and SW3 toggles the green and red\r\n" \
        "   LED on and off respectively, and prints \"SW2\" and \"SW3\" \r\n" \
        "   in the terminal application.\r\n");

    while(1)
    {
        // --------------------------------------------------------------------
        if(serial_rxcnt() > 0)
        {
            // Use the getchar() function from the standard library to read a
            // character from the LPUART0 receive buffer. Note that this will
            // work, because the function function for reading characters is
            // redirected to the serial_getchar() function in the retarget.c
            // file.
            int c = getchar();

            // Use the printf() function from the standard library to print the
            // received character to the console. Note that this will
            // work, because the function function for writing characters is
            // redirected to the serial_putchar() function in the retarget.c
            // file.
            printf("Received: %c\r\n", c);

            // Check if the received character is a 'r', 'g' or 'b' and toggle
            // the corresponding LED.
            if((c == 'r') || (c == 'R'))
            {
                // Red LED toggle
                led_red_toggle();
            }

            if((c == 'g') || (c == 'G'))
            {
                // Green LED toggle
                led_green_toggle();
            }

            if((c == 'b') || (c == 'B'))
            {
                // Blue LED toggle
                led_blue_toggle();
            }
        }

        // --------------------------------------------------------------------
        if(sw2_pressed())
        {
            // SW2 pressed: toggle green LED and print message
            led_green_toggle();
            printf("SW2\r\n");
        }

        // --------------------------------------------------------------------
        if(sw3_pressed())
        {
            // SW3 pressed: toggle red LED and print message
            led_red_toggle();
            printf("SW3\r\n");
        }
    }
}

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------
