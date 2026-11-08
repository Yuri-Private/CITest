/*
 * LED blink pattern of the CITest firmware, kept free of hardware access so it can be unit-tested on a PC.
 */
#ifndef BLINK_H
#define BLINK_H

#include <stdint.h>

#define BLINK_STARTUP_TOGGLES   20U   /* 2 s of fast blinking after reset: 20 toggles of 100 ms */
#define BLINK_STARTUP_DELAY_MS  100U
#define BLINK_RUN_DELAY_MS      250U  /* then one blink every 500 ms: two toggles of 250 ms */

/* Time to wait after LED toggle number `toggle` (0 = the first toggle after reset). */
uint32_t blink_delay_ms(uint32_t toggle);

/* Toggle counter for the next toggle: counts up to the end of the start-up phase, then stays there,
   so the start-up phase never repeats when the counter would wrap. */
uint32_t blink_next_toggle(uint32_t toggle);

#endif /* BLINK_H */
