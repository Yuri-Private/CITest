#include "blink.h"

uint32_t blink_delay_ms(uint32_t toggle)
{
  return (toggle < BLINK_STARTUP_TOGGLES) ? BLINK_STARTUP_DELAY_MS : BLINK_RUN_DELAY_MS;
}

uint32_t blink_next_toggle(uint32_t toggle)
{
  return (toggle < BLINK_STARTUP_TOGGLES) ? toggle + 1U : BLINK_STARTUP_TOGGLES;
}
