
#include <fmt_comms.h>

void comm_init(void);

void comm_sendVersion(uint32_t versionRaw);

#define USE_Reset
void handleReset(Reset msg);

void comm_handleTelemetry(void);