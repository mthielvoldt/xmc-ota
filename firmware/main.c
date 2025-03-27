
#ifdef XMC4700_F144x2048
#include "XMC4700.h"
#endif

#include "config/ISR_Config.h"
#include "config/project_comms.h"
#include "frequency.h"

#include <wolfboot/wolfboot.h>
// #include <fmt_spi.h>
#include "spi_init.h"
#include <fmt_rx.h>
#include <fmt_periodic_port.h> // TODO: decompose this in FMT
#include <fmt_update.h>

void onDownloadComplete(void);
void onDownloadStart(void);

void main(void)
{
  spi_init();

  fmt_setFirstPageReceivedCallback(onDownloadStart);
  fmt_setDownloadFinishCallback(onDownloadComplete);

  comm_init();

  // Set periodicA to 1kHz frequency.
  initPeriodicISR(
      CCU40,
      CCU40_CC40,
      XMC_CCU4_SLICE_PRESCALER_128,
      PRESCALE_128_TICKS_IN_1MS,
      periodicA_IRQn,
      periodicA_priority);

  comm_sendVersion(wolfBoot_current_firmware_version());

  for (;;)
    ;
}

// 1kHz
void periodicA()
{
  comm_handleTelemetry();
  fmt_handleRx();
}

void onDownloadStart(void)
{
  wolfBoot_update_trigger();
}

/** fmt_update Download_finished callback
 * Executed when an image download completes.
 * This requests the wolfboot bootloader boot to this image on next boot.
 * This request comes via a status footer (trailer) written to flash partition.
 */
void onDownloadComplete(void)
{
  NVIC_SystemReset();
}