
#ifdef XMC4700_F144x2048
#include "XMC4700.h"
#endif

#include "ISR_Config.h"
#include "project_comms.h"
#include "frequency.h"

#include <fmt_spi.h>
#include <fmt_rx.h>
#include <fmt_periodic_port.h> // TODO: decompose this in FMT

void main(void) {

  extern ARM_DRIVER_SPI Driver_SPI4;

  spiCfg_t spiConfig = {
      .spiModule = &Driver_SPI4,
      .msgWaitingInput = {RTE_IOC_P1_0_ERU0}, // ERU0.3
      .msgWaitingOut = 3, // must call msgWaitingISR from ERU0_3_IRQHandler
      .msgWaitingIRQn = ERU0_3_IRQn,
      .clearToSendInput = {RTE_IOC_P0_4_ERU0}, // ERU0.2
      .clearToSendOut = 2, // must call clearToSendISR from ERU0_2_IRQHandler
      .clearToSendIRQn = ERU0_2_IRQn,
      .baudHz = 1000000,
      .busMode = BUS_MODE_MAIN,
      .ssActiveLow = true,
      .spiIrqPriority = spiTxBuf_priority, // TODO: check if this is used.
  };
  fmt_initSpi(spiConfig);

  comm_init((portPin_t){.port = XMC_GPIO_PORT5, .pin = 9U});

  // Set periodicA to 1kHz frequency.
  initPeriodicISR(
    CCU40,
    CCU40_CC40,
    XMC_CCU4_SLICE_PRESCALER_128,
    PRESCALE_128_TICKS_IN_1MS,
    periodicA_IRQn,
    periodicA_priority);

  for (;;);
}

// 1kHz
void periodicA()
{
  comm_handleTelemetry();
  fmt_handleRx();
}