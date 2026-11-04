/*
 * CITest firmware: blinks LD2 (blue, PB7) on a NUCLEO-F767ZI every 500 ms.
 * Bare metal, no HAL: the MCU runs from its 16 MHz internal oscillator (HSI) after reset.
 */
#include <stdint.h>

#include "version.h"

#define REG32(addr)   (*(volatile uint32_t *)(addr))

/* Reset and clock control */
#define RCC_AHB1ENR   REG32(0x40023830UL)
#define RCC_GPIOBEN   (1UL << 1)

/* GPIO port B */
#define GPIOB_MODER   REG32(0x40020400UL)
#define GPIOB_ODR     REG32(0x40020414UL)
#define LED_PIN       7U

/* SysTick */
#define SYST_CSR      REG32(0xE000E010UL)
#define SYST_RVR      REG32(0xE000E014UL)
#define SYST_CVR      REG32(0xE000E018UL)

#define CORE_CLOCK_HZ 16000000UL

/* Version strings in flash: readable from the .bin/.hex or with a debugger. Kept by the linker script. */
__attribute__((section(".fw_version"), used))
static const char fw_version[] = "CITest FW " FW_VERSION " for app " UI_VERSION;

static volatile uint32_t ms_ticks;

void SysTick_Handler(void)
{
  ms_ticks++;
}

static void delay_ms(uint32_t ms)
{
  uint32_t start = ms_ticks;
  while ((uint32_t)(ms_ticks - start) < ms)
  {
  }
}

int main(void)
{
  RCC_AHB1ENR |= RCC_GPIOBEN;
  (void)RCC_AHB1ENR;  /* read back: the clock must be running before the port is touched */

  GPIOB_MODER = (GPIOB_MODER & ~(3UL << (LED_PIN * 2U))) | (1UL << (LED_PIN * 2U));

  SYST_RVR = CORE_CLOCK_HZ / 1000U - 1U;  /* 1 ms tick */
  SYST_CVR = 0U;
  SYST_CSR = 7U;                          /* processor clock, interrupt, enable */

  for (;;)
  {
    GPIOB_ODR ^= 1UL << LED_PIN;
    delay_ms(250U);  /* toggle twice per blink: one blink every 500 ms */
  }
}
