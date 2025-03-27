//***************************************************************************************
//  MSP430 Output a controlled bipolar (-/+) current
//
//  Description; Toggle P1.1 and P1.2 in complemntary fashion to create a
//  bipolar current ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430FR2433
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->  LED, DRIVER_ENABLE
//            |             P1.1|--> DRIVER_CHANNEL_1
//            |             P1.2|--> DRIVER_CHANNEL_2
//  Mohamed Elazab
//  MetroHealth Research Institute
//  Mar 2025
//***************************************************************************************

#include <msp430.h>

int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
  PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
                        // to activate previously configured port settings
  P1DIR |= 0x07; //  Set P1.0, P1.1 & P1.2 as outputs         P1DIR = xxxx x111
  // P1.0 => DRIVER_ENABLE
  // P1.1 => DRIVER_CHANNEL_1
  // P1.2 => DRIVER_CHANNEL_2
  P1OUT = 0x03;
  // DRIVER_ENABLE -> HIGH and light LED
  // DRIVER_CHANNEL_1 -> HIGH
  // DRIVER_CHANNEL_2 -> LOW

  for (;;) {
    volatile unsigned long i; // volatile to prevent optimization

    P1OUT ^= 0x06; // Toggle P1.1 & P1.2 using exclusive-OR
    i = 100000;    // SW Delay
    do
      i--;
    while (i != 0);
  }

  return 0;
}
