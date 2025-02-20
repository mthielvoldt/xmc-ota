#include "project_comms.h"
#include <fmt_log.h>
#include <fw_update.h>
#include <fmt_flash.h>

#define CALLS_PER_FULL_ROTATION 1000U

// optimization: stop storing the config.
static portPin_t led;

void comm_init(portPin_t ledPin)
{
  led = ledPin;
  XMC_GPIO_Init(led.port, led.pin, &gpOutPinConfig);
}

void comm_handleTelemetry(void)
{
  static uint32_t count = 0;
  static uint32_t rotations = 0;

  // execute periodic telemetry sends in round-robin.
  switch (count++)
  {
  case 0:
  {
    if (++rotations > 100) rotations = 0;

    fmt_sendMsg((const Top){
        .which_sub = Top_ExampleTlm_tag,
        .sub = {
            .ExampleTlm = {
                .currentMa = rotations}}});

    break;
  }
  case CALLS_PER_FULL_ROTATION:
    count = 0;
    break;
  }
}