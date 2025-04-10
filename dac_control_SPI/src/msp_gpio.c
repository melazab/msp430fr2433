#include "msp_gpio.h"
#include <msp430.h>
/**
 * @brief Initialize MSP430 GPIOs
 */
void init_gpio(void) {
  // Configure LED
  P1DIR |= LED_PIN; // Set LED pin as output
  LED_OFF();        // Initially turn LED off
}
