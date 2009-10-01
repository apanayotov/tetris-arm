/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
/// Basic DC Motor Control
///
/// Krzysztof Sierszecki, Sept 2009
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
//         Local definitions
//------------------------------------------------------------------------------

#define BOARD_ADC_FREQ 5000000
#define ADC_VREF       3300  // 3.3 * 1000

/// PWM frequency in Hz.
#define PWM_FREQUENCY               20000

/// Maximum duty cycle value.
#define MAX_DUTY_CYCLE              255

/// Minimum duty cycle value (cannot be 0 or 1 for the SAM7S, c.f. errata)
#if defined(at91sam7s16) || defined(at91sam7s161) || defined(at91sam7s32) \
    || defined(at91sam7s321) || defined(at91sam7s64) || defined(at91sam7s128) \
    || defined(at91sam7s256) || defined(at91sam7s512) || defined(at91sam7a3)

    #define MIN_DUTY_CYCLE          2
#else
    #define MIN_DUTY_CYCLE          0
#endif

#define DISABLE_INTERRUPTS \
   asm volatile ( \
         "DisInt:             \n\t" \
         "mrs  r0, CPSR       \n\t"             /* Get CPSR                   */\
         "orr  r0, r0, #0xC0  \n\t"             /* Disable IRQ, FIQ           */\
         "msr  CPSR, r0       \n\t"             /* Write back modified value  */\
         "mrs  r0, CPSR       \n\t"             /* Back from INT? Get CPSR    */\
         "ands r0, r0, #0xC0  \n\t"             /* Check interrupts flags     */\
         "beq  DisInt         \n\t"             /* Interrupts still enabled?  */\
         : : : "r0" );                          /* Clobber list:  r0          */

#define ENABLE_INTERRUPTS \
   asm volatile ( \
         "mrs  r0, CPSR       \n\t"             /* Get CPSR                   */\
         "bic  r0, r0, #0xC0  \n\t"             /* Enable IRQ, FIQ            */\
         "msr  CPSR, r0       \n\t"             /* Write back modified value  */\
         : : : "r0" );                          /* Clobber list:  r0          */

//------------------------------------------------------------------------------
//         Local variables
//------------------------------------------------------------------------------

/// Break signal PIO pin definition.
#define PIN_BREAK  {(1 << 29), AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
/// Direction signal PIO pin definition.
#define PIN_DIRECTION  {(1 << 30), AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PINS_EXT  PIN_BREAK, PIN_DIRECTION

/// TIOA2 TC pin definition.
#define PIN_TC_TIOA2_EXT  {1 << 27, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// TIOB2 TC PWM1 pin definition.
#define PIN_TC_TIOB2_EXT  {1 << 28, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define PINS_TC  PIN_TC_TIOA2_EXT, PIN_TC_TIOB2_EXT

/// PIO pins to configure.
static const Pin pins[] = {
    PINS_DBGU,
    PINS_JOYSTICK,
    PINS_SWITCH,
    PINS_EXT,
    PIN_PWMC_PWM2,
    PINS_TC,
    PIN_ADC0_ADC2
};

#define PINS_INDEX_DBGU             0
#define PINS_INDEX_JOYSTICK_UP      1
#define PINS_INDEX_JOYSTICK_DOWN    2
#define PINS_INDEX_JOYSTICK_RIGHT   3
#define PINS_INDEX_JOYSTICK_LEFT    4
#define PINS_INDEX_JOYSTICK_BUTTON  5
#define PINS_INDEX_SWITCH1          6
#define PINS_INDEX_SWITCH2          7
#define PINS_INDEX_BREAK            8
#define PINS_INDEX_DIRECTION        9
#define PINS_INDEX_PWM2             10
#define PINS_INDEX_TIOA2            11
#define PINS_INDEX_TIOB2            12
#define PINS_INDEX_SENSE            13

enum MotorDirection {
   Left,
   Right
};

enum MotorBreak {
   On,
   Off
};

enum SwitchState {
   Released,
   Pressed
};

static volatile UBYTE periodFlag = 0;

static UWORD mSpeed = 0;
static UWORD setpoint = 0;
static unsigned short controlSignal = 0;
static enum MotorDirection mDirection = Left;
static enum MotorBreak mBreak = Off;
static unsigned int mCurrent = 0;
static enum SwitchState switchBreak = Released, switchDirection = Released;

//------------------------------------------------------------------------------
//         Local functions
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Convert a digital value in milivolt
/// /param valueToconvert Value to convert in miliAmp
//-----------------------------------------------------------------------------
static unsigned int ConvHex2mA( unsigned int valueToConvert )
{
    return( (ADC_VREF * valueToConvert)/0x3FF);
}

//------------------------------------------------------------------------------
/// Wait time in ms
//------------------------------------------------------------------------------
void UTIL_Loop(unsigned int loop)
{
    while(loop--);	
}


void UTIL_WaitTimeInMs(unsigned int mck, unsigned int time_ms)
{
    register unsigned int i = 0;
    i = (mck / 1000) * time_ms;
    i = i / 3;
    UTIL_Loop(i);
}

//------------------------------------------------------------------------------
/// Wait time in us
//------------------------------------------------------------------------------
void UTIL_WaitTimeInUs(unsigned int mck, unsigned int time_us)
{
    volatile unsigned int i = 0;
    i = (mck / 1000000) * time_us;
    i = i / 3;
    UTIL_Loop(i);
}

//  ****************************************************************************
//     Interrupt handler for the Periodic Interval Timer (PIT)
//  ****************************************************************************

void ISR_System_Interrupt(void)
{
   unsigned int status;
   volatile static UWORD counter = 0;

   // Get PIT status
   status = PIT_GetStatus();

   // Periodic interval has elapsed?
   if ((status & AT91C_PITC_PITS) == AT91C_PITC_PITS) {
      // acknowledge the interrupt
      PIT_GetPIVR();

      if (++counter == 10) {
         // Toggle 'LED'
         LCDSetRect(3, 125, 6, 128, FILL, WHITE);
         //Backlight(1);
      }
      else if (counter == 20) {
         LCDSetRect(3, 125, 6, 128, FILL, BLACK);
         //Backlight(0);
         counter = 0;
      }

      periodFlag = 1;
   }
}

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Outputs a PWM to control motor speed.
//------------------------------------------------------------------------------
int main(void)
{
   static char s[32];

   PIO_Configure(pins, PIO_LISTSIZE(pins));

   TRACE_CONFIGURE(DBGU_STANDARD, 115200, BOARD_MCK);
   printf("-- Basic DC Motor Control Project %s --\n\r", SOFTPACK_VERSION);
   printf("-- %s\n\r", BOARD_NAME);
   printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

   UTIL_WaitTimeInMs(BOARD_MCK, 1000);
   UTIL_WaitTimeInUs(BOARD_MCK, 1000);

   // Enable PWMC peripheral clock
   AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PWMC;

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

   ADC_EnableChannel(AT91C_BASE_ADC, CHANNEL_ADC_2);

   // Settings:
   // - 20kHz PWM period (PWM_FREQUENCY)

   // Set clock A to run at 100kHz * MAX_DUTY_CYCLE (clock B is not used)
   PWMC_ConfigureClocks(PWM_FREQUENCY * MAX_DUTY_CYCLE, 0, BOARD_MCK);

   // Configure PWMC channel for PWM2 (left-aligned)
   PWMC_ConfigureChannel(CHANNEL_PWM2, AT91C_PWMC_CPRE_MCKA, AT91C_PWMC_CALG, 0);
   PWMC_SetPeriod(CHANNEL_PWM2, MAX_DUTY_CYCLE);
   PWMC_SetDutyCycle(CHANNEL_PWM2, MAX_DUTY_CYCLE - MIN_DUTY_CYCLE);

   // Enable channel #2
   PWMC_EnableChannel(CHANNEL_PWM2);

   // Init PIT and AIC
   // Configure PIT for 100 ms (us, MHz)
   PIT_Init(1, BOARD_MCK/10);

   // Enable PIT interrupt
   AIC_ConfigureIT(AT91C_ID_SYS, 0, ISR_System_Interrupt);
   AIC_EnableIT(AT91C_ID_SYS);
   PIT_EnableIT();

   // Clear the screen
   LCDClearScreen();

   ENABLE_INTERRUPTS;

   // Infinite loop
   while ( TRUE ) {
      if ( periodFlag ) {
         // Clear the flag
         periodFlag = 0;

         // Sense the Motor Current
         mCurrent = ConvHex2mA(ADC_GetConvertedData(AT91C_BASE_ADC, CHANNEL_ADC_2));
         ADC_StartConversion(AT91C_BASE_ADC);

         // Read Switch1 - Break
         if ( switchBreak == Released && !PIO_Get(&pins[PINS_INDEX_SWITCH1]) ) {
            if ( mBreak == Off ) {
               PIO_Set(&pins[PINS_INDEX_BREAK]);
               mBreak = On;
            }
            else {
               PIO_Clear(&pins[PINS_INDEX_BREAK]);
               mBreak = Off;
            }
            switchBreak = Pressed;
         }
         else if ( PIO_Get(&pins[PINS_INDEX_SWITCH1]) ) {
            switchBreak = Released;
         }

         // Read Switch2 - Direction
         if ( switchDirection == Released && !PIO_Get(&pins[PINS_INDEX_SWITCH2]) ) {
            if ( mDirection == Left ) {
               PIO_Set(&pins[PINS_INDEX_DIRECTION]);
               mDirection = Right;
            }
            else {
               PIO_Clear(&pins[PINS_INDEX_DIRECTION]);
               mDirection = Left;
            }
            switchDirection = Pressed;
         }
         else if ( PIO_Get(&pins[PINS_INDEX_SWITCH2]) ) {
            switchDirection = Released;
         }

         // Read Joystick Up
         if ( !PIO_Get(&pins[PINS_INDEX_JOYSTICK_UP]) ) {
            if (controlSignal < MAX_DUTY_CYCLE) {
             controlSignal++;
             // Set duty cycle of PWM2
             PWMC_SetDutyCycle(CHANNEL_PWM2, MAX_DUTY_CYCLE - controlSignal);
            }
         }
         // Read Joystick Down
         if ( !PIO_Get(&pins[PINS_INDEX_JOYSTICK_DOWN]) ) {
            if (controlSignal > MIN_DUTY_CYCLE) {
             controlSignal--;
             // Set duty cycle of PWM2
             PWMC_SetDutyCycle(CHANNEL_PWM2, MAX_DUTY_CYCLE - controlSignal);
            }
         }

         DISABLE_INTERRUPTS;

         sprintf(s, "Break: %s", mBreak == Off ? "Off" : "On ");
         LCDPutStr(s, 12, 10, SMALL, WHITE, BLACK);
         sprintf(s, "Direction: %s", mDirection == Left ? "Left " : "Right");
         LCDPutStr(s, 22, 10, SMALL, WHITE, BLACK);
         sprintf(s, "Setpoint: %3u", setpoint);
         LCDPutStr(s, 32, 10, SMALL, WHITE, BLACK);
         sprintf(s, "Speed: %3u", mSpeed);
         LCDPutStr(s, 42, 10, SMALL, WHITE, BLACK);
         sprintf(s, "Control: %3u", controlSignal);
         LCDPutStr(s, 52, 10, SMALL, WHITE, BLACK);
         sprintf(s, "Current: %4u mA", mCurrent);
         LCDPutStr(s, 62, 10, SMALL, WHITE, BLACK);

         ENABLE_INTERRUPTS;

      }
   }
}

