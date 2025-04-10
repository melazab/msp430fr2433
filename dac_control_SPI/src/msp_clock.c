#include "msp_clock.h"
#include <msp430.h>

void init_clock(void) {
  // Configure MSP430 clock
  __bis_SR_register(SCG0);  // Disable FLL
  CSCTL3 = SELREF__REFOCLK; // Set REFOCLK as FLL reference source
  CSCTL0 = 0;               // Clear DCO and MOD registers
  CSCTL1 &= ~(DCORSEL_7);   // Clear DCO frequency select bits first
  CSCTL1 |= DCORSEL_3;      // Set DCOCLK = 8MHz
  CSCTL2 = FLLD_1 + 121;    // FLLD = 1, DCODIV = DCO/2 = 4MHz
  __delay_cycles(3);
  __bic_SR_register(SCG0); // Enable FLL
  while (CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1))
    ; // Poll until FLL is locked

  CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK; // Set ACLK = REFOCLK = 32768Hz,
                                             // DCOCLK as MCLK and SMCLK source
  CSCTL5 &= ~(DIVM0 | DIVS0); // Remove division for full speed (4MHz)
}
