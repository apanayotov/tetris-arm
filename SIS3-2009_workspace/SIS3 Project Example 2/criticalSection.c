//------------------------------------------------------------------------------
//         Critical Section
//------------------------------------------------------------------------------
//
// Author: Krzysztof Sierszecki  February 24, 2008
//
//------------------------------------------------------------------------------

#include "criticalSection.h"
#include "typedef.h"

volatile UWORD nestedCritical = 0;

void EnterCritical(void) 
{
   DISABLE_INTERRUPTS;
   nestedCritical++;
}

void ExitCritical(void) 
{
   if ( !--nestedCritical ) {
      ENABLE_INTERRUPTS;
   }
}
