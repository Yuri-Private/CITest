/*
 * Minimal startup for the STM32F767ZI (Cortex-M7, 110 interrupts).
 * Sets up .data and .bss, enables the FPU (the code is built for the hard-float ABI) and calls main().
 */
#include <stdint.h>

#define SCB_CPACR  (*(volatile uint32_t *)0xE000ED88UL)

/* Symbols from the linker script */
extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _estack;

int main(void);
void Reset_Handler(void);

void Default_Handler(void)
{
  for (;;)
  {
  }
}

void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)    __attribute__((weak, alias("Default_Handler")));

#define CORE_VECTORS 16
#define IRQ_VECTORS  110

/* Vector table: initial stack pointer, then the exception and interrupt handlers. */
__attribute__((section(".isr_vector"), used))
void (* const vector_table[CORE_VECTORS + IRQ_VECTORS])(void) =
{
  (void (*)(void))&_estack,
  Reset_Handler,
  NMI_Handler,
  HardFault_Handler,
  MemManage_Handler,
  BusFault_Handler,
  UsageFault_Handler,
  0, 0, 0, 0,
  SVC_Handler,
  DebugMon_Handler,
  0,
  PendSV_Handler,
  SysTick_Handler,
  [CORE_VECTORS ... CORE_VECTORS + IRQ_VECTORS - 1] = Default_Handler,
};

void Reset_Handler(void)
{
  SCB_CPACR |= 0xFUL << 20;  /* full access to coprocessors CP10 and CP11 (FPU) */
  __asm volatile ("dsb\n\tisb");

  uint32_t *src = &_sidata;
  uint32_t *dst = &_sdata;
  while (dst < &_edata)
  {
    *dst++ = *src++;
  }
  for (dst = &_sbss; dst < &_ebss; )
  {
    *dst++ = 0U;
  }

  main();
  for (;;)
  {
  }
}
