#include "hardware/dac63004w.h"
#include "msp_clock.h"
#include "msp_delay.h"
#include "msp_gpio.h"
#include "msp_spi.h"
#include <msp430.h>
#include <stddef.h> /* For NULL definition */

void init_spi_peripherals(void);

// DAC context
dac63004w_context_t *dac_ctx;
/**
 * @brief Main function
 */
int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

  // Initialize systems
  init_clock();

  // Disable the GPIO power-on default high-impedance mode FIRST!
  PM5CTL0 &= ~LOCKLPM5;

  init_gpio();

  init_spi_peripherals();

  dac_ctx->vref = 3.3;
  dac_ctx->mode = DAC_MODE_VOLTAGE;
  // Initialize DAC
  dac_init(dac_ctx);

  while (1) {
    dac_write_voltage(dac_ctx, 0, 2);

    delay_ms(50);
  }
}

/**
 * @brief Initialize SPI and other peripherals
 */
void init_spi_peripherals(void) {
  // Define SPI pin configuration
  spi_pin_config_t spi_pins = {
      .mosi_port = 1,
      .mosi_pin = 4, // P1.4 = MOSI/SDI
      .miso_port = 1,
      .miso_pin = 5, // P1.5 = MISO/SDO (not used but configured)
      .sclk_port = 1,
      .sclk_pin = 6, // P1.6 = SCLK
      .cs_port = 1,
      .cs_pin = 7 // P1.7 = CS/SYNC
  };

  // Define SPI configuration
  spi_config_t spi_config = {
      .clock_divider = 8, // SMCLK/8 = 500kHz (since we're running at 4MHz now)
      .mode = 1,          // SPI mode 1 (CPOL=0, CPHA=1)
      .bit_order = 0      // MSB first
  };

  // Initialize SPI
  msp_spi_init(&spi_pins, &spi_config);
}
