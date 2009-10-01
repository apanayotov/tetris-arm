//  ****************************************************************************
//                          main.c
//
//      Author:  James P Lynch        August 30, 2007
//               Krzysztof Sierszecki February 26, 2008
//  ****************************************************************************

//  ****************************************************************************
//                Header Files
//  ****************************************************************************

#include <board.h>
#include <pio/pio.h>
#include <pit/pit.h>
#include <aic/aic.h>
#include <dbgu/dbgu.h>
#include <utility/trace.h>
#include "lcd/lcd.h"

#include <stdio.h>

#include "typedef.h"
#include "bmp/bmpSkyline.h"
#include "criticalSection.h"


//  ****************************************************************************
//     Consts
//  ****************************************************************************
//  PIO pins configuration
static const Pin joystick_pins[] = {PINS_JOYSTICK};
static const Pin switch_pins[]   = {PINS_SWITCH};
static const Pin debug_pins[]    = {PINS_DBGU};


//  ****************************************************************************
//     Interrupt handler for the Periodic Interval Timer (PIT)
//  ****************************************************************************

void ISR_System_Interrupt(void)
{
   unsigned int status;
   volatile static UBYTE heartBeat = 0;
   volatile static UWORD counter = 0;

   // Get PIT status
   status = PIT_GetStatus();

   // Periodic interval has elapsed?
   if ((status & AT91C_PITC_PITS) == AT91C_PITC_PITS) {
      // acknowledge the interrupt
      PIT_GetPIVR();

      if (counter++ > 10) {
         // Toggle 'LED'
         if (heartBeat) {
            LCDSetRect(3, 125, 6, 128, FILL, WHITE);
            //Backlight(1);
            heartBeat = 0;
         } else {
            LCDSetRect(3, 125, 6, 128, FILL, BLACK);
            //Backlight(0);
            heartBeat = 1;
         }
         counter = 0;
      }
   }
}


//  ****************************************************************************
//     Main
//  ****************************************************************************

int main(void)
{
   UBYTE x, y;
   static char s[32];

   // Init input pins
   PIO_Configure(joystick_pins, PIO_LISTSIZE(joystick_pins));
   PIO_Configure(switch_pins, PIO_LISTSIZE(switch_pins));
   PIO_Configure(debug_pins, PIO_LISTSIZE(debug_pins));

   TRACE_CONFIGURE(DBGU_STANDARD, 115200, BOARD_MCK);
   printf("-- SIS3 Project Example 1 %s --\n\r", SOFTPACK_VERSION);
   printf("-- %s\n\r", BOARD_NAME);
   printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

   // Initialize SPI interface to LCD
   InitSpi();

   // Init LCD
   InitLcd();

   // Init PIT and AIC
   // Configure PIT for 100 ms (us, MHz)
   PIT_Init(1, BOARD_MCK/10);

   // Enable PIT interrupt
   AIC_ConfigureIT(AT91C_ID_SYS, 0, ISR_System_Interrupt);
   AIC_EnableIT(AT91C_ID_SYS);
   PIT_EnableIT();

   EnterCritical();
   // Clear the screen
   LCDClearScreen();

   // Display nice pictures
   LCDWrite130x130bmp((unsigned char *)bmpSkyline);

   LCDPutStr("Hello World", 10, 10, SMALL, WHITE, BLACK);
   ExitCritical();

   //while(1); // stop here

   x = 2;
   y = 0;
   // loop forever
   while (1) {
      if ( !PIO_Get(&joystick_pins[JOYSTICK_UP]) && (x > 2))
         x--;

      if ( !PIO_Get(&joystick_pins[JOYSTICK_DOWN]) && (x < 131-10))
         x++;

      if ( !PIO_Get(&joystick_pins[JOYSTICK_RIGHT]) && (y < 129-10))
         y++;

      if ( !PIO_Get(&joystick_pins[JOYSTICK_LEFT]) && (y > 0))
         y--;

      if ( !PIO_Get(&joystick_pins[JOYSTICK_BUTTON]) ) {
         x = 60;
         y = 60;
      }

      if ( !PIO_Get(&switch_pins[SWITCH1]) ) {
         EnterCritical();
         // test screen
         LCDClearScreen();

         LCDSetPixel(30, 120, RED);
         LCDSetPixel(34, 120, GREEN);
         LCDSetPixel(38, 120, BLUE);
         LCDSetPixel(42, 120, WHITE);

         // draw some characters
         LCDPutChar('E', 10, 10, SMALL, WHITE, BLACK);

         // draw a string
         LCDPutStr("Hello World", 60, 10, SMALL, WHITE, BLACK);
         LCDPutStr("Hello World", 40, 10, MEDIUM, ORANGE, BLACK);
         LCDPutStr("Hello World", 20, 10, LARGE, PINK, BLACK);

         // draw a filled box
         LCDSetRect(120, 60, 80, 80, FILL, BROWN);

         // draw a empty box
         LCDSetRect(120, 85, 80, 105, NOFILL, CYAN);

         // draw some lines
         LCDSetLine(120, 10, 120, 50, YELLOW);
         LCDSetLine(120, 50, 80, 50, YELLOW);
         LCDSetLine(80, 50, 80, 10, YELLOW);
         LCDSetLine(80, 10, 120, 10, YELLOW);

         LCDSetLine(120, 85, 80, 105, YELLOW);
         LCDSetLine(80, 85, 120, 105, YELLOW);

         // draw a circle
         LCDSetCircle(65, 100, 10, RED);
         ExitCritical();
      }
      if ( !PIO_Get(&switch_pins[SWITCH2]) ) {
         EnterCritical();
         LCDWrite130x130bmp((unsigned char *)bmpSkyline);
         ExitCritical();
      }

      sprintf(s, "x:%03u y:%03u", x, y);

      EnterCritical();
      LCDPutStr(s, 110, 10, SMALL, WHITE, BLACK);

      LCDSetRect(x, y, x+10, y+10, FILL, BLUE);
      LCDSetRect(x, y, x+10, y+10, NOFILL, WHITE);
      ExitCritical();

      Delay(100000);
   }
}




