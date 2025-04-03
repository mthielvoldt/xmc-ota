#include "config/project_comms.h"
#include <fmt_gpio.h>
#include "config/gpio_pcbDetails.h"
#include <wolfboot/wolfboot.h>

#define CALLS_PER_FULL_ROTATION 500U

void comm_init(void)
{
  fmt_initGpioOutPin(LED_0_PIN_ID, OUTPUT_MODE_PUSH_PULL);
}

void comm_sendVersion(uint32_t versionRaw)
{
  fmt_sendMsg((const Top){
      .which_sub = Top_Version_tag,
      .sub = {
          .Version = {
              .major = versionRaw >> 24,
              .minor = (versionRaw >> 16) & 0xFF,
              .patch = versionRaw & 0xFFFF,
          }}});
}
void handleReset(Reset msg)
{
  NVIC_SystemReset();
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
    rotations++;

    fmt_sendMsg((const Top){
        .which_sub = Top_ExampleTlm_tag,
        .sub = {
            .ExampleTlm = {
                .currentMa = rotations}}});

    break;
  }
  case 1:
  {
    comm_sendVersion(wolfBoot_current_firmware_version());
  }
  case 5:
  {
    fmt_setPin(LED_0_PIN_ID, OUTPUT_TOGGLE);
    break;
  }
  case CALLS_PER_FULL_ROTATION:
    count = 0;
    break;
  }
}