
#include "gpio_common.h"

#include <messages.pb.h>
#include <fmt_spi.h>  // must be here to provide fmt_rx with correct driver.

void comm_init(portPin_t ledPin);

void comm_handleTelemetry(void);