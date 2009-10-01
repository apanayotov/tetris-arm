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

# force a peripheral RESET AT91C_RSTC_RCR
#monitor mww 0xfffffd00 0xa5000004 

# toggle the remap register to place RAM at 0x00000000
monitor mww 0xffffff00 0x01       

# set the PC to 0x00000000
#monitor reg pc 0x00000000        

monitor arm7_9 sw_bkpts enable    # enable use of software breakpoints

# needed for gdb 6.8 and higher
set mem inaccessible-by-default off

load
break main
continue
