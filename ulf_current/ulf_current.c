//******************************************************************************
//  MSP430FR24xx Timer-based Waveform Generator with Adjustable On-Times
//
//  Description: Generate complementary waveforms on P1.0 (red LED, cathodic)
//  and P1.1 (green LED, anodic) with adjustable on-times for each phase.
//
//  ACLK = TACLK = 32768Hz, MCLK = SMCLK = 8MHz/2
//
//                MSP430FR2433
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//            |             P1.0|--> RED LED (Cathodic)
//            |             P1.1|--> GREEN LED (Anodic)
//
//  Mohamed Elazab
//  MetroHealth Research Institute
//  Apr 2025
//******************************************************************************/
#include <msp430.h>

// Define timing constants (adjust these values as needed)

#define CATHODIC_PW 10
#define ANODIC_PW 10

#define SCALING_FACTOR 0.5
#define CATHODIC_ON_TIME ((CATHODIC_PW) * (SCALING_FACTOR))
#define ANODIC_ON_TIME ((ANODIC_PW) * (SCALING_FACTOR))

// Global variables
volatile unsigned int counter = 0;
volatile unsigned char currentPhase = 0;

int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // Stop WDT

  // Configure clock
  __bis_SR_register(SCG0);  // disable FLL
  CSCTL3 = SELREF__REFOCLK; // Set REFOCLK as FLL reference source
  CSCTL0 = 0;               // clear DCO and MOD registers
  CSCTL1 &= ~(DCORSEL_7);   // Clear DCO frequency select bits first
  CSCTL1 |= DCORSEL_3;      // Set DCOCLK = 8MHz
  CSCTL2 = FLLD_1 + 121;    // FLLD = 1, by default, DCODIV = DCO/2 = 4MHz
  __delay_cycles(3);
  __bic_SR_register(SCG0); // enable FLL
  while (CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1))
    ; // Poll until FLL is locked

  CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK; // set ACLK = REFOCLK = 32768Hz,
                                             // DCOCLK as MCLK and SMCLK source
  CSCTL5 |= DIVM0 | DIVS0;                   // SMCLK = MCLK = DCODIV = 4MHz

  // Configure GPIO
  P1DIR |= BIT0 | BIT1; // Set P1.0 and P1.1 as outputs
  P1OUT |= BIT0;        // Turn RED LED on initially (Cathodic phase)
  P1OUT &= ~BIT1;       // Turn GREEN LED off initially

  // Disable the GPIO power-on default high-impedance mode to activate
  // previously configured port settings
  PM5CTL0 &= ~LOCKLPM5;

  // Configure Timer_A
  TA0CTL = TASSEL_1 | MC_2 | TACLR |
           TAIE; // ACLK, continuous mode, clear TAR, enable interrupt

  __bis_SR_register(LPM3_bits | GIE); // Enter LPM3, enable interrupts
  __no_operation();                   // For debugger
}

// Timer0_A3 Interrupt Vector (TAIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER0_A1_VECTOR))) TIMER0_A1_ISR(void)
#else
#error Compiler not supported!
#endif
{
  switch (__even_in_range(TA0IV, TA0IV_TAIFG)) {
  case TA0IV_NONE:
    break; // No interrupt
  case TA0IV_TACCR1:
    break; // CCR1 not used
  case TA0IV_TACCR2:
    break; // CCR2 not used
  case TA0IV_TAIFG:
    counter++; // Increment counter

    if (currentPhase == 0) { // Currently in Cathodic phase (Red LED)
      if (counter >= CATHODIC_ON_TIME) {
        // Switch to Anodic phase
        P1OUT &= ~BIT0;   // Turn off RED LED
        P1OUT |= BIT1;    // Turn on GREEN LED
        counter = 0;      // Reset counter
        currentPhase = 1; // Switch to Anodic phase
      }
    } else { // Currently in Anodic phase (Green LED)
      if (counter >= ANODIC_ON_TIME) {
        // Switch to Cathodic phase
        P1OUT |= BIT0;    // Turn on RED LED
        P1OUT &= ~BIT1;   // Turn off GREEN LED
        counter = 0;      // Reset counter
        currentPhase = 0; // Switch to Cathodic phase
      }
    }
    break;
  default:
    break;
  }
}
