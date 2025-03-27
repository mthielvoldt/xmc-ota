#include <fmt_spi.h>
// #include "config/spi_pcbDetails.h"
#include "config/ioc_pcbDetails.h"
#include "config/ISR_Config.h"


bool spi_init(void)
{
  extern ARM_DRIVER_SPI Driver_SPI4; // USIC2_0

  spiCfg_t spiConfig = {
    .spiModuleId = 4,
    .spiModule = &Driver_SPI4,
    .msgWaitingIocId = IOC_14_RTE_INDEX, // IOC_14: P1_0 on XMC4700
    .msgWaitingIocOut = IOC_14_to_ISR_3,
    .clearToSendIocId = IOC_4_RTE_INDEX, // IOC_4:  P0_4 on XMC4700
    .clearToSendIocOut = IOC_4_to_ISR_2,
    .baudHz = 1000000,
    .busMode = BUS_MODE_MAIN,
    .ssActiveLow = true,
    .irqPriority = spiTxBuf_priority, // TODO: check if this is used.
  };

  return fmt_initSpi(spiConfig);
}