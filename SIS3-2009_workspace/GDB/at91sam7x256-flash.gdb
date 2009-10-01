target remote localhost:3333

# syntax: monitor OpenOCD_command

monitor reset                     # hard-reset         
monitor sleep 500
monitor poll                      # poll the target for its current state

monitor soft_reset_halt           # halt the processor and wait

monitor armv4_5 core_state arm    # select the core to ARM mode

# watchdog disable (AT91C_WDTC_WDMR)
monitor mww 0xfffffd44 0xa0008000 

# enable user reset (AT91C_RSTC_RMR)
monitor mww 0xfffffd08 0xa5000001 

# set flash wait state (AT91C_MC_FMR)
monitor mww 0xffffff60 0x00320100 
monitor sleep 10

# enable main oscillator (AT91C_PMC_MOR)
monitor mww 0xfffffc20 0xa0000601 
monitor sleep 10

# set PLL register (AT91C_PMC_PLLR)
monitor mww 0xfffffc2c 0x00481c0e 
monitor sleep 10

# set master clock to PLL (AT91C_PMC_MCKR)
monitor mww 0xfffffc30 0x00000007 
monitor sleep 100

# convert all breakpoints to hardware breakpoints
monitor arm7_9 force_hw_bkpts enable 

#symbol-file target-flash.elf

# needed for gdb 6.8 and higher
set mem inaccessible-by-default off

thbreak main
continue
