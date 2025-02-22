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