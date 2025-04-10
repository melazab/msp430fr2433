#include "msp_delay.h"
#include <msp430.h>
/**
 * @brief Delay function (approximate milliseconds)
 *
 * TODO: Implement delay function using MSP430's Timer0
 * @param ms Milliseconds to delay
 */
void delay_ms(uint16_t ms) {
  // At 4MHz, each cycle is 0.25us, so 4000 cycles = 1ms
  while (ms--) {
    __delay_cycles(4000);
  }
}
