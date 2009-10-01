//------------------------------------------------------------------------------
//         Critical Section
//------------------------------------------------------------------------------
//
// Author: Krzysztof Sierszecki  February 24, 2008
//
//------------------------------------------------------------------------------

#ifndef __CRITICALSECTION_H__
#define __CRITICALSECTION_H__

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
   
void EnterCritical(void); 
void ExitCritical(void); 

#endif /*CRITICALSECTION_H_*/
