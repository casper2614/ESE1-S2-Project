#include <stdlib.h>
#include <board.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>

#include "serial.h"
#include "lpuart2.h"
#include "math.h"

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



int KAMERRUIMTE = 1;
char *IBEACON_UUID[] = {IBEACON_SOLDEERUIMTE, IBEACON_Bjorn, "2", "3", "4", "5", "6", "7", "8"};
// Match is de specefieke disc die we nodig hebben
char disc[512];
int signal;
static int hm10_Signal_Sterkte(void)
{
    char rx_copy[RX_BUFFER_SIZE];
    strncpy(rx_copy, rx_buffer, RX_BUFFER_SIZE);
    
    char *disc = strstr(rx_copy, IBEACON_UUID[KAMERRUIMTE]);
    if (disc == NULL) 
    {
        return 0; // de iBeacon is niet gevonden
    }


    char *p = disc;
    for (int i = 0; i < 3; i++) {
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

    serial_init(115200);
    lpuart2_init(9600);

    printf("GeovVenture Geocache\r\n");
    printf("%s build %s %s\r\n", TARGETSTR, __DATE__, __TIME__);

    while (1)
    {
        __WFI();

        if ((ms % 100) == 0)
        {
            rx_index = 0;
            memset(rx_buffer, 0, sizeof(rx_buffer));
            hm10_send("AT+DISI?");
            // de delay hier moet nog verbetered worden en vervangen worden door een timer Floris zij dat die het zou doen.
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