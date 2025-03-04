# Getting FW Update working
- set_update_sector_flag() calls will likely clobber nearby flags. Space out.

# Debugging new BL
- testPassSaved() returned true when maybe it shouldn't have?
  - passed because binAssemble filled that pmem with FF's. - add comment. 

# Debugging image download
## Rx buffer overlfow
- hit breakpoint in Tx dropped with buffer size = 4. CPU stall.
Resolution:
- removed breakpoint, adding a TxDropped counter
- increased buffer size from 4 to 10.

## Comms lockup
1. Comms locking up after ~40 pages.m (exact number varies 15->70)
2. It does this after receiving the 9th packet in an 10-packet page.
3. SPI sub-select never recovers to hi-state. (msg never ends)
  - indicates issue is with main, not sub. This is auto-driven

Suspects
- Data reading ISR can't keep up. -> Rx FIFO fills, stalls USIC
  - Previously, Tx-dropped Breakpoint intervened before this would happen. 

To Check
- Is there a mode where USIC locks up to prevent data loss? **IMPORTANT**
- What relevant debug resources does USIC offer?
- What is happening in the handleSpiEventISR context? (does it take long?)
  - It seems pretty quick. CRC, enqueue, set-bit. 
- What are the priorities in-play?  Can lengthier ISR block spiEventHandler?
  - PeriodicA: 30
  - CTS Ioc:   
  - Msg Ioc:  
  - SPIevent: 63, 0 (woah, Hard-coded!)
  - RxQueue max sender: 16
  - TxQueue max sender: 16

### Todo
- 

## Importing Firment 
### Imports that should be optional
- Ghost Probe
  - gp_config.h
  - messages in messages.proto
    - RunScanCtl
    - data one
  - probes.proto
  - ghost_probe.c
- Waveform stuff
  - fmt_waveform_cfg.h

### Things that just aren't right
- Break fmt_periodic_xmc.h into port and generic sections.  
- Defines needed by fmt unclear.  like spiTxBuf_priority
- gpio_common.h/c  Feels required and tied to HW.

### Opportunities
- move the port linking in CML to firment CML.

## Integrating App + wolfBoot
- get fw_update.h info from generated target.h

## Wolfboot Gotcha Abatement
- CMake build should check for and delete target.h if it's in wolfBoot/src