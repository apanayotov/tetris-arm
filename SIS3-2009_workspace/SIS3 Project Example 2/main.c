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
#include <adc/adc.h>
#include <pio/pio.h>
#include <pit/pit.h>
#include <aic/aic.h>
#include <dbgu/dbgu.h>
#include <pwmc/pwmc.h>
#include <utility/trace.h>
#include "lcd/lcd.h"

#include <stdio.h>

#include "typedef.h"
#include "bmp/bmpFullSpectrum.h"
#include "criticalSection.h"


//  ****************************************************************************
//     Defines
//  ****************************************************************************
#define BOARD_ADC_FREQ 5000000
// PWM Settings
#define MAX_FREQUENCY      11000 // Maximum frequency in Hz -> sound bandwidth

//  ****************************************************************************
//     Consts
//  ****************************************************************************
//  PIO pins configuration
static const Pin joystick_pins[] = {PINS_JOYSTICK};
static const Pin switch_pins[]   = {PINS_SWITCH};
static const Pin debug_pins[]    = {PINS_DBGU};
static const Pin other_pins[]    = {PIN_AUDIO_OUT};

//  ****************************************************************************
//     Globals
//  ****************************************************************************
volatile unsigned int freq;

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
//     Audio handler
//  ****************************************************************************

void ISR_PWM_Interrupt(void)
{
   static unsigned int old_freq = 0;

   // Interrupt on channel #0
   if ( AT91C_BASE_PWMC->PWMC_ISR & AT91C_PWMC_CHID0 )
   {
      // Generate square wave
      if ( freq != old_freq ) {
         // If PWN channel is enabled either Period or Duty Cycle can be updated
         //  -> not both
         //PWMC_SetPeriod(AUDIO_OUT_CHANNEL, freq);
         AT91C_BASE_PWMC->PWMC_CH[CHANNEL_PWM_AUDIO_OUT].PWMC_CPRDR = freq;
         //PWMC_SetDutyCycle(AUDIO_OUT_CHANNEL, freq >> 1);
         AT91C_BASE_PWMC->PWMC_CH[CHANNEL_PWM_AUDIO_OUT].PWMC_CDTYR = freq >> 1;

         old_freq = freq;
      }
   }
}


//  ****************************************************************************
//     Main
//  ****************************************************************************

int main(void)
{
   static UBYTE backlight = 1;
   static unsigned int temp, trim;
   static char s[32];

   // Init input pins
   PIO_Configure(joystick_pins, PIO_LISTSIZE(joystick_pins));
   PIO_Configure(switch_pins, PIO_LISTSIZE(switch_pins));
   PIO_Configure(debug_pins, PIO_LISTSIZE(debug_pins));
   PIO_Configure(other_pins, PIO_LISTSIZE(other_pins));

   TRACE_CONFIGURE(DBGU_STANDARD, 115200, BOARD_MCK);
   printf("-- SIS3 Project Example 2 %s --\n\r", SOFTPACK_VERSION);
   printf("-- %s\n\r", BOARD_NAME);
   printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

   // Initialize SPI interface to LCD
   InitSpi();

   // Init LCD
   InitLcd();

   ADC_Initialize( AT91C_BASE_ADC,
                   AT91C_ID_ADC,
                   AT91C_ADC_TRGEN_DIS,
                   0,
                   AT91C_ADC_SLEEP_NORMAL_MODE,
                   AT91C_ADC_LOWRES_10_BIT,
                   BOARD_MCK,
                   BOARD_ADC_FREQ,
                   10,
                   1200);

   ADC_EnableChannel(AT91C_BASE_ADC, CHANNEL_ADC_TEMP);
   ADC_EnableChannel(AT91C_BASE_ADC, CHANNEL_ADC_TRIM);

   ADC_StartConversion(AT91C_BASE_ADC);

   // Init PIT and AIC
   // Configure PIT for 100 ms (us, MHz)
   PIT_Init(1, BOARD_MCK/10);

   // PWM Initialization
   // Enable PWMC peripheral clock
   AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PWMC;

   // Settings:
   // Set clock A to run at 2 * MAX_FREQUENCY (clock B is not used)
   PWMC_ConfigureClocks(2 * MAX_FREQUENCY, 0, BOARD_MCK);

   // Configure PWMC channel #0 (left-aligned)
   PWMC_ConfigureChannel(CHANNEL_PWM_AUDIO_OUT, AT91C_PWMC_CPRE_MCKA, 0, 0);
   //PWMC_SetPeriod(CHANNEL_PWM_AUDIO_OUT, MAX_DUTY_CYCLE);
   //PWMC_SetDutyCycle(CHANNEL_PWM_AUDIO_OUT, MAX_DUTY_CYCLE);

   // Configure interrupt on channel #0
   AIC_ConfigureIT(AT91C_ID_PWMC, 2, ISR_PWM_Interrupt);  // priority 2
   AIC_EnableIT(AT91C_ID_PWMC);
   PWMC_EnableChannelIt(CHANNEL_PWM_AUDIO_OUT);

   // Enable PWMC channel #0
   //PWMC_EnableChannel(AUDIO_OUT_CHANNEL);

   // Enable PIT interrupt
   AIC_ConfigureIT(AT91C_ID_SYS, 0, ISR_System_Interrupt);
   AIC_EnableIT(AT91C_ID_SYS);
   PIT_EnableIT();

   EnterCritical();
   // Clear the screen
   LCDClearScreen();

   // Display nice pictures
   LCDWrite130x130bmp((unsigned char *)bmpFullSpectrum);
   LCDPutStr("Press SWITCH1", 110, 1, SMALL, WHITE, BLACK);

   ExitCritical();

   //while(1); // stop here

   // loop forever
   while (1) {

      if ( !PIO_Get(&switch_pins[SWITCH1]) ) {
         EnterCritical();

         if ( backlight ) {
            Backlight(0);
            backlight = 0;

            PWMC_DisableChannel(CHANNEL_PWM_AUDIO_OUT);
         } else {
            Backlight(1);
            backlight = 1;

            PWMC_SetPeriod(CHANNEL_PWM_AUDIO_OUT, freq);
            PWMC_SetDutyCycle(CHANNEL_PWM_AUDIO_OUT, freq >> 1);
            PWMC_EnableChannel(CHANNEL_PWM_AUDIO_OUT);
         }
         ExitCritical();
      }

      temp = ADC_GetConvertedData(AT91C_BASE_ADC, CHANNEL_ADC_TEMP);
      trim = ADC_GetConvertedData(AT91C_BASE_ADC, CHANNEL_ADC_TRIM);
      // Start the conversion
      ADC_StartConversion(AT91C_BASE_ADC);

      temp = (temp * 10) / 19; // Celsius *10
      snprintf(s, 14, "Temp: %3u.%1u", (temp/10), (temp%100)%10);

      freq = ((218 * trim) / 1023) + 2; // frequencies from 100 - 11000 Hz

      EnterCritical();
      LCDPutStr(s, 12, 1, SMALL, WHITE, BLACK);
      snprintf(s, 14, "Trim: %5u", trim);
      LCDPutStr(s, 21, 1, SMALL, WHITE, BLACK);
      snprintf(s, 14, "Freq: %5u", (2 * MAX_FREQUENCY) / freq);
      LCDPutStr(s, 30, 1, SMALL, WHITE, BLACK);

      ExitCritical();


      Delay(1000000);
   }
}




