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

    printf("1. Verify that after microcontroller reset a message appears\r\n" \
        "   in the terminal application.\r\n" \
        "2. Verify that typing the characters 'r', 'g' and 'b' in the \r\n" \
        "   terminal application toggles the corresponding RBG LED.\r\n" \
        "3. Verify that pressing SW2 and SW3 toggles the green and red\r\n" \
        "   LED on and off respectively, and prints \"SW2\" and \"SW3\" \r\n" \
        "   in the terminal application.\r\n");

    
*/
/******************************************************************************/
#include <MCXA153.h>
#include <board.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "leds.h"
#include "switches.h"
#include "serial.h"
#include "lpuart2.h"
#include "math.h"
#include <stdlib.h>
// Ontvangst buffer
#define RX_BUFFER_SIZE 512
static char rx_buffer[RX_BUFFER_SIZE];
static uint16_t rx_index = 0;

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
static volatile uint32_t ms = 0;
int delay = 4000;
static void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 12000; i++)
    {
        __asm("nop");
    }
}

static void hm10_send(const char *cmd)
{
    for (uint16_t i = 0; cmd[i] != '\0'; i++)
    {
        lpuart2_putchar((uint8_t)cmd[i]);
    }
    printf("[TX] %s\r\n", cmd);
}
static void hm10_receive(void)
{
    while (lpuart2_rxcnt() > 0)
    {
        char c = (char)lpuart2_getchar();
        if (rx_index < RX_BUFFER_SIZE - 1)
        {
            rx_buffer[rx_index++] = c;
            rx_buffer[rx_index] = '\0';
        }
    }
}
// -----------------------------------------------------------------------------
// de major en minor nummers van iBeacon in kamers
typedef struct{
    char major[5];
    char minor[5];
} majorMinor_t;
/*const majorMinor_t BEACON_SOLDEERRUIMTE = {"0AEA", "0037"};
const majorMinor_t BEACON_COMPONENTENBALIE = {"0AEA", "0026"};
const majorMinor_t BEACON_MUUR = {"0AEA", "0032"};*/
const majorMinor_t RUIMTE_1 = {"0B01", "0004"};
const majorMinor_t RUIMTE_2 = {"0B01", "0005"};
const majorMinor_t RUIMTE_3 = {"0B01", "0006"};
const majorMinor_t RUIMTE_4 = {"0B01", "0007"};
const majorMinor_t RUIMTE_5 = {"0B01", "0008"};
const majorMinor_t RUIMTE_6 = {"0B01", "0009"};
const majorMinor_t RUIMTE_7 = {"0B01", "000A"};
// major nummer van de ibeacons beneden is 0AEA
// minor nummer muur beneden 0x0032
// minor nummer componentenbalie 0x0026
// minor nummer soldeerruimte 0x0037
// major nummer van de locatie die we bezocht hebben is 0b01
// minor nummers van die beacons zijn, 0004, 0005, 0006, 0007, 0008, 0009, 000A. (volgorde staat nog niet vast)
//-----------------------------------------------------------------------------

int KAMERRUIMTE = 0;
// de structs in de array zijn placeholders, vervang deze met namen van ruimtes
majorMinor_t IBEACON_MM[] = {RUIMTE_1, RUIMTE_2, RUIMTE_3, RUIMTE_4, RUIMTE_5, RUIMTE_6, RUIMTE_7};
// Match is de specefieke disc die we nodig hebben
char disc[512];
int signal;
static int hm10_Signal_Sterkte(void)
{
    char rx_copy[RX_BUFFER_SIZE];
    strncpy(rx_copy, rx_buffer, RX_BUFFER_SIZE);
    
    char *disc = strstr(rx_copy, IBEACON_MM[KAMERRUIMTE].major);
    if (disc == NULL) 
    {
        printf("Major niet gevonden");
        return 0; // de iBeacon is niet gevonden
    }
    printf("%s\n", disc);
    disc = strstr(disc, IBEACON_MM[KAMERRUIMTE].minor);
    if (disc == NULL) 
    {
        printf("Minor  niet gevonden");
        return 0; // de iBeacon is niet gevonden
    }
    printf("%s\n", disc);


    char *p = disc;
    for (int i = 0; i < 2; i++) {
        p = strchr(p, ':');
        if (p == NULL) {
            return 0; // onverwacht formaat
        }
        p++; // naar het begin van het volgende veld
    }

    // p wijst nu naar het veld met de signaalsterkte
    sscanf(p, "%d", &signal); 
    printf("Signal sterkte: %d\r\n", signal);
    return abs(signal);

}
// -----------------------------------------------------------------------------
/*
wat zijn de adressen van de iBeacons die worden gebruikt door Hugo?
   #define IBEACON_X "de adress van de iBeacon"
   wat ook zou kunnen opzig is dat we een array maken van alle adressen van alle iBeacons die we krijgen van hugo,
   iBeacon_UUID[1, 2, 3, 4, 5, 6, 7, 8, 9, 10] = {"adres1", "adres2", "adres3", "adres4", "adres5", "adres6", "adres7", "adres8", "adres9", "adres10"}
   en dan in de functie voor de Ibeacon detectie kan je het een getal geven van 1 tot 10
   en dan geeft die gewoon de signal sterkte van die iBeacon specefiek
   dus in de loop is het dan iets van r = iBeacon_Search(i); met i gelijk aan de room dat ze in zitten
   en dan kan je met een if statement zoals
   if (r > 100) {
       de code voor de eerste kamer
   }
   en dan aan de einde van die puzzel als ze hem goed hebben kan je gewoon i++ doen
   dit lijkt mij het simpelste en meest intuitive manier lol

*/

// -----------------------------------------------------------------------------
int main(void)
{
    SysTick->CTRL = 0;
    uint32_t SysTick_Config(uint32_t ticks);
    SysTick_Config(48000);
    __enable_irq();

    leds_init();
    sw_init();
    serial_init(115200);
    lpuart2_init(9600);

    printf("Template example\r\n");
    printf("%s build %s %s\r\n", TARGETSTR, __DATE__, __TIME__);

    hm10_send("AT");
    delay_ms(500);
    hm10_receive();
    printf("[RX] %s\r\n\r\n", rx_buffer);
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));

    hm10_send("AT+RESET");
    delay_ms(1500);
    hm10_receive();
    printf("[RX] %s\r\n\r\n", rx_buffer);
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));

    hm10_send("AT+ROLE1");
    delay_ms(500);
    hm10_receive();
    printf("[RX] %s\r\n\r\n", rx_buffer);
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));

    hm10_send("AT+IMME1");
    delay_ms(500);
    hm10_receive();
    printf("[RX] %s\r\n\r\n", rx_buffer);
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));

    hm10_send("AT+VERR?");
    delay_ms(1500);
    hm10_receive();
    printf("[RX] %s\r\n\r\n", rx_buffer);
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
    printf("Initialisatie klaar. Start scannen...\r\n");

    while (1)
    {
        __WFI();

        if ((ms % 100) == 0)
        {
            rx_index = 0;
            memset(rx_buffer, 0, sizeof(rx_buffer));
            hm10_send("AT+DISI?");
            // de delay hier hoeft NIET weggewerkt te worden, de code die hierna komt kan niet uitgevoerd worden totdat hm10_receive af is.
            for (uint32_t t = 0; t < 4000; t++) // Wacht 4 seconde
            {
                hm10_receive();
                delay_ms(1);
            }
            
            printf("[RX] '%s'\r\n", rx_buffer);

            // rx_index = 0;
            // memset(rx_buffer, 0, sizeof(rx_buffer));
            // ssri is de absulute waarde van de signaal sterkte van de Ibeacon die je zoekt
            int ssri = hm10_Signal_Sterkte(); 
            printf("Signal sterkte: %d\r\n", ssri);

            if (0 < ssri) 
            {
                printf("Ibeacon Gevonden \r\n");

            } else {
                printf("Ibeacon Niet Gevonden \r\n");
            }
            /* 
            if ((KAMERRUIMTE == 1) && (ssri > 40)) {
                code voor puzzel 1
                KAMERRUIMTE++;
            } else if ((KAMERRUIMTE == 2) && (ssri > 40)) {
                code voor puzzel 2
                KAMERRUIMTE++;
            } else if ((KAMERRUIMTE == 3) && (ssri > 40)) {
                code voor puzzel 3
                KAMERRUIMTE++;
            } else if ((KAMERRUIMTE == 4) && (ssri > 40)) {
                code voor puzzel 4
                KAMERRUIMTE++;
            } else if ((KAMERRUIMTE == 5) && (ssri > 40)) {
                code voor puzzel 5
                KAMERRUIMTE++;
            } else { 
                printf("Kameruimte is niet herkent \r\n");
                printf("KAMERRUIMTE: %d \r\n", KAMERRUIMTE);
            }
            */
        }
    }
}
void SysTick_Handler(void)
{
    ms++;
}
