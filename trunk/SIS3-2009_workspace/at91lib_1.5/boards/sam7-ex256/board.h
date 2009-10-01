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

// Edited: Krzysztof Sierszecki,  Sept 2009

//------------------------------------------------------------------------------
/// \dir
/// !Purpose
/// 
/// Definition and functions for using AT91SAM7X-related features, such
/// has PIO pins, memories, etc.
/// 
/// !Usage
/// -# The code for booting the board is provided by board_cstartup.S and
///    board_lowlevel.c.
/// -# For using board PIOs, board characteristics (clock, etc.) and external
///    components, see board.h.
/// -# For manipulating memories (remapping, SDRAM, etc.), see board_memories.h.
//------------------------------------------------------------------------------
 
//------------------------------------------------------------------------------
/// \unit
/// !Purpose
/// 
/// Definition of SAM7-EX256 characteristics, AT91SAM7X-dependant PIOs and
/// external components interfacing.
/// 
/// !Usage
/// -# For operating frequency information, see "SAM7-EX256 - Operating frequencies".
/// -# For using portable PIO definitions, see "SAM7-EX256 - PIO definitions".
/// -# Several USB definitions are included here (see "SAM7-EX256 - USB device").
//------------------------------------------------------------------------------

#ifndef BOARD_H 
#define BOARD_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "at91sam7x256/AT91SAM7X256.h"

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM7-EX256 - Board Description"
/// This page lists several definition related to the board description
///
/// !Definitions
/// - BOARD_NAME

/// Name of the board.
#define BOARD_NAME "SAM7-EX256"
/// Board definition.
#define sam7ex256
/// Family definition.
#define at91sam7x

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM7-EX256 - Operating frequencies"
/// This page lists several definition related to the board operating frequency
/// (when using the initialization done by board_lowlevel.c).
/// 
/// !Definitions
/// - BOARD_MAINOSC
/// - BOARD_MCK

/// Frequency of the board main oscillator.
#define BOARD_MAINOSC           18432000

/// Master clock frequency (when using board_lowlevel.c).
#define BOARD_MCK               48000000
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ADC
//------------------------------------------------------------------------------
/// ADC clock frequency, at 10-bit resolution (in Hz)
#define ADC_MAX_CK_10BIT         5000000
/// ADC clock frequency, at 8-bit resolution (in Hz)
#define ADC_MAX_CK_8BIT          8000000
/// Startup time max, return from Idle mode (in µs)
#define ADC_STARTUP_TIME_MAX       20
/// Track and hold Acquisition Time min (in ns)
#define ADC_TRACK_HOLD_TIME_MIN   600


//------------------------------------------------------------------------------
/// \page "SAM7-EX256 - USB device"
/// This page lists constants describing several characteristics (controller
/// type, D+ pull-up type, etc.) of the USB device controller of the chip/board.
/// 
/// !Constants
/// - BOARD_USB_UDP
/// - BOARD_USB_PULLUP_ALWAYSON
/// - BOARD_USB_NUMENDPOINTS
/// - BOARD_USB_ENDPOINTS_MAXPACKETSIZE
/// - BOARD_USB_ENDPOINTS_BANKS
/// - BOARD_USB_BMATTRIBUTES

/// Chip has a UDP controller.
#define BOARD_USB_UDP

/// Indicates the D+ pull-up is always connected.
#define BOARD_USB_PULLUP_ALWAYSON

/// Number of endpoints in the USB controller.
#define BOARD_USB_NUMENDPOINTS                  6

/// Returns the maximum packet size of the given endpoint.
#define BOARD_USB_ENDPOINTS_MAXPACKETSIZE(i)    ((((i) == 4) || ((i) == 5)) ? 256 : (((i) == 0) ? 8 : 64))

/// Returns the number of FIFO banks for the given endpoint.
#define BOARD_USB_ENDPOINTS_BANKS(i)            ((((i) == 0) || ((i) == 3)) ? 1 : 2)

/// USB attributes configuration descriptor (bus or self powered, remote wakeup)
#define BOARD_USB_BMATTRIBUTES                  USBConfigurationDescriptor_BUSPOWERED_NORWAKEUP
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM7-EX256 - PIO definitions"
/// This pages lists all the pio definitions contained in board.h. The constants
/// are named using the following convention: PIN_* for a constant which defines
/// a single Pin instance (but may include several PIOs sharing the same
/// controller), and PINS_* for a list of Pin instances.

/// List of all DBGU pin definitions.
#define PINS_DBGU  {0x18000000, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// Switch button #1 definition.
#define PIN_SWITCH1         {AT91C_PIO_PB24, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
/// Switch button #2 definition.
#define PIN_SWITCH2         {AT91C_PIO_PB25, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
/// List of all switch button definitions.
#define PINS_SWITCH         PIN_SWITCH1, PIN_SWITCH2
/// Switch button #1 index
#define SWITCH1             0
/// Switch button #2 index
#define SWITCH2             1

/// Joystick definition.
#define PIN_JOYSTICK_UP     {AT91C_PIO_PA9,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_JOYSTICK_DOWN   {AT91C_PIO_PA8,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_JOYSTICK_RIGHT  {AT91C_PIO_PA14, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_JOYSTICK_LEFT   {AT91C_PIO_PA7,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_JOYSTICK_BUTTON {AT91C_PIO_PA15, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}

#define PINS_JOYSTICK       PIN_JOYSTICK_UP, PIN_JOYSTICK_DOWN, PIN_JOYSTICK_RIGHT, PIN_JOYSTICK_LEFT, PIN_JOYSTICK_BUTTON

#define JOYSTICK_UP         0
#define JOYSTICK_DOWN       1
#define JOYSTICK_RIGHT      2
#define JOYSTICK_LEFT       3
#define JOYSTICK_BUTTON     4

/// USART0 RXD pin definition.
#define PIN_USART0_RXD  {1 << 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART0 TXD pin definition.
#define PIN_USART0_TXD  {1 << 1, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART0 SCK pin definition.
#define PIN_USART0_SCK  {1 << 2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART0 RTS pin definition
#define PIN_USART0_RTS  {1 << 3, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART0 CTS pin definition
#define PIN_USART0_CTS  {1 << 4, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// SPI0 MISO pin definition.
#define PIN_SPI0_MISO   {1 << 16, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// SPI0 MOSI pin definition.
#define PIN_SPI0_MOSI   {1 << 17, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
/// SPI0 SPCK pin definition.
#define PIN_SPI0_SPCK   {1 << 18, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
/// List of SPI0 pin definitions (MISO, MOSI & SPCK).
#define PINS_SPI0       PIN_SPI0_MISO, PIN_SPI0_MOSI, PIN_SPI0_SPCK
/// SPI0 chip select 0 pin definition.
#define PIN_SPI0_NPCS0  {1 << 12, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
/// SPI0 chip select 1 pin definition.
#define PIN_SPI0_NPCS1  {1 << 13, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}

/// PWMC PWM0 pin definition.
#define PIN_PWMC_PWM0  {1 << 19, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// PWMC PWM1 pin definition.
#define PIN_PWMC_PWM1  {1 << 20, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// PWMC PWM2 pin definition.
#define PIN_PWMC_PWM2  {1 << 21, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// PWMC PWM3 pin definition.
#define PIN_PWMC_PWM3  {1 << 22, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}

/// PWMC PWM0 pin definition.
#define PIN_PWMC_PWM0_EXT  {1 << 27, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
/// PWMC PWM1 pin definition.
#define PIN_PWMC_PWM1_EXT  {1 << 28, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
/// PWMC PWM2 pin definition.
#define PIN_PWMC_PWM2_EXT  {1 << 29, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
/// PWMC PWM3 pin definition.
#define PIN_PWMC_PWM3_EXT  {1 << 30, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}

// PWM channels
#define CHANNEL_PWM0 (0)
#define CHANNEL_PWM1 (1)
#define CHANNEL_PWM2 (2)
#define CHANNEL_PWM3 (3)

// Audio output pin definition
#define PIN_AUDIO_OUT               PIN_PWMC_PWM0
// LCD Backlight pin definition
#define PIN_LCD_BACKLIGHT           PIN_PWMC_PWM1
// Audio out PWM channel
#define CHANNEL_PWM_AUDIO_OUT       (CHANNEL_PWM0)
// LCD Backlight PWM channel
#define CHANNEL_PWM_LCD_BACKLIGHT   (CHANNEL_PWM1)

/// ADC_AD0 pin definition.
#define PIN_ADC0_ADC0 {1 << 27, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
/// ADC_AD1 pin definition.
#define PIN_ADC0_ADC1 {1 << 28, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
/// ADC_AD2 pin definition.
#define PIN_ADC0_ADC2 {1 << 29, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
/// ADC_AD3 pin definition.
#define PIN_ADC0_ADC3 {1 << 30, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
/// Pins ADC
#define PINS_ADC PIN_ADC0_ADC0, PIN_ADC0_ADC1, PIN_ADC0_ADC2, PIN_ADC0_ADC3

// AD0 ADC channel
#define CHANNEL_ADC_0   (0)
// AD0 ADC channel
#define CHANNEL_ADC_1   (1)
// AD0 ADC channel
#define CHANNEL_ADC_2   (2)
// AD0 ADC channel
#define CHANNEL_ADC_3   (3)

// Temperature sensor ADC channel
#define CHANNEL_ADC_TEMP   (5)
// Trim potentiometer ADC channel
#define CHANNEL_ADC_TRIM   (6)
// Microphone input ADC channel
#define CHANNEL_ADC_MIC    (7)

/// TWI pins definition.
#define PINS_TWI  {0x00000C00, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// CAN Definition
/// TXD: Transmit data input
#define PINS_CAN_TRANSCEIVER_TXD  {1<<20, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// RXD: Receive data output
#define PINS_CAN_TRANSCEIVER_RXD  {1<<19, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM7-EX256 - External components"
/// This page lists definitions related to external on-board components.
/// 
/// !SD SPI
/// - BOARD_SD_SPI_BASE
/// - BOARD_SD_SPI_ID
/// - BOARD_SD_SPI_PINS
/// - SD_WP_PIN
/// - SD_CP_PIN
/// - BOARD_SD_NPCS
///
/// !EMAC
/// - BOARD_EMAC_PIN_PWRDN
/// - BOARD_EMAC_MODE_RMII
/// - BOARD_EMAC_PINS
/// - BOARD_EMAC_PIN_TEST
/// - BOARD_EMAC_PIN_RMII
/// - BOARD_EMAC_PINS_PHYAD
/// - BOARD_EMAC_PIN_10BT
/// - BOARD_EMAC_PIN_RPTR
/// - BOARD_EMAC_RST_PINS
/// - BOARD_EMAC_RUN_PINS

/// Base address of the SPI peripheral connected to the SD card.
#define BOARD_SD_SPI_BASE   AT91C_BASE_SPI0
/// Identifier of the SPI peripheral connected to the SD card.
#define BOARD_SD_SPI_ID     AT91C_ID_SPI0
/// SD card Write Protection pin definition
#define PIN_SD_WP           {AT91C_PIO_PB22, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
/// SD card Card Present pin definition
#define PIN_SD_CP           {AT91C_PIO_PB23, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
/// List of pins to configure to access the SD card
#define BOARD_SD_SPI_PINS   PINS_SPI0, PIN_SPI0_NPCS1, PIN_SD_WP, PIN_SD_CP
/// NPCS number
#define BOARD_SD_NPCS       1

/// Board EMAC Power Down control pin
#define BOARD_EMAC_PIN_PWRDN {(1<<18), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
/// Board EMAC mode - RMII/MII ( 1/0 )
#define BOARD_EMAC_MODE_RMII 0 // MII is the default mode
/// The PIN list of PIO for EMAC
#define BOARD_EMAC_PINS     {0x3FFFF, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// The power up reset latch PIO for PHY
#define BOARD_EMAC_PIN_TEST   {(1<<15), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define BOARD_EMAC_PIN_RMII   {(1<<16), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
// We force the address
//(1<<5) PHY address 0, (1<<6) PHY address 1, (1<<13) PHY address 2,
//(1<<14) PHY address 3, (1<<4) PHY address 4
#define BOARD_EMAC_PINS_PHYAD {(1<<6)|(1<<13)|(1<<14)|(1<<4),\
                               AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT},\
                              {(1<<5), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#define BOARD_EMAC_PIN_10BT   {(1<<17), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define BOARD_EMAC_PIN_RPTR   {(1<< 7), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
/// The PIN Configure list for EMAC on power up reset (MII)
#define BOARD_EMAC_RST_PINS BOARD_EMAC_PINS_PHYAD, \
                            BOARD_EMAC_PIN_TEST, BOARD_EMAC_PIN_RMII, \
                            BOARD_EMAC_PIN_10BT, BOARD_EMAC_PIN_RPTR

/// The runtime pin configure list for EMAC
#define BOARD_EMAC_RUN_PINS BOARD_EMAC_PINS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM7-EX256 - Memories"
/// This page lists definitions related to internal & external on-board memories.
/// 
/// !Embedded Flash
/// - BOARD_FLASH_EFC
/// - BOARD_FLASH_IAP_ADDRESS

/// Indicates chip has an EFC.
#define BOARD_FLASH_EFC
/// Address of the IAP function in ROM.
#define BOARD_FLASH_IAP_ADDRESS         0x300008
//------------------------------------------------------------------------------

#endif //#ifndef BOARD_H

