#include "hardware/dac63004w.h"
#include "hardware/msp_spi.h"
#include <msp430.h>
#include <stddef.h> /* For NULL definition */
#include <stdio.h>

// LED for status indication
#define LED_PORT P1OUT
#define LED_PIN BIT0
#define LED_ON() (LED_PORT |= LED_PIN)
#define LED_OFF() (LED_PORT &= ~LED_PIN)
#define LED_TOGGLE() (LED_PORT ^= LED_PIN)

// Function prototypes
void init_clock(void);
void init_gpio(void);
void init_peripherals(void);
void delay_ms(uint16_t ms);
void dac63004w_init(dac63004w_context_t *ctx);

// Global DAC context
dac63004w_context_t *dac_ctx = {.vref = 3.3, .mode = DAC_MODE_VOLTAGE};

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

  // Initialize SPI peripherals
  init_peripherals();

  // Initialize DAC with debug output
  dac63004w_init(*dac_ctx);

  while (1) {
    dac_write_voltage(dac_ctx, 0, 2);
    dac_write_voltage(dac_ctx, 1, 2);
    dac_write_voltage(dac_ctx, 2, 2);
    dac_write_voltage(dac_ctx, 3, 2);

    delay_ms(50);
  }
}

/**
 * @brief DAC initialization
 */
void dac63004w_init(float reference_voltage) {
  dac_status_t status;

  // Initialize the DAC context
  dac_ctx.vref = reference_voltage;
  dac_ctx.mode = DAC_MODE_VOLTAGE;

  // Step 1: Software reset
  status = dac_write_register(DAC_REG_COMMON_TRIGGER, 0x0A00);
  if (status != DAC_SUCCESS) {
    while (1)
      ; // Halt on error
  }

  // Extra delay after reset
  delay_ms(10);

  // Step 2: Set interface config
  status = dac_write_register(DAC_REG_INTERFACE, 0x0000);
  if (status != DAC_SUCCESS) {
    while (1)
      ; // Halt on error
  }

  // Step 3: Set device mode
  status = dac_write_register(DAC_REG_DEVICE_MODE, 0x0000);
  if (status != DAC_SUCCESS) {
    while (1)
      ; // Halt on error
  }

  // Step 4: Configure channel 0 gain
  status = dac_write_register(DAC_REG_DAC0_VOUT_CONFIG,
                              0x0400); // DAC_VOUT_GAIN_1X_VDD_REFERENCE
  if (status != DAC_SUCCESS) {
    while (1)
      ; // Halt on error
  }

  // Step 5: Enable internal reference and set normal operation
  status = dac_write_register(DAC_REG_COMMON_CONFIG,
                              0x1000); // Only enable internal reference
  if (status != DAC_SUCCESS) {
    while (1)
      ; // Halt on error
  }

  // Step 6: Set DAC0 to 0V
  status = dac_write_register(DAC_REG_DAC0_DATA, 0x0000);
  if (status != DAC_SUCCESS) {
    while (1)
      ; // Halt on error
  }

  // Step 7: Trigger LDAC to update outputs
  status =
      dac_write_register(DAC_REG_COMMON_TRIGGER, 0x0080); // DAC_LDAC_TRIGGER
  if (status != DAC_SUCCESS) {
    while (1)
      ; // Halt on error
  }

  // Extra delay after initialization
  delay_ms(10);
}

/**
 * @brief Initialize clock system
 */
void init_clock(void) {
  // Configure clock system
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

/**
 * @brief Initialize GPIO
 */
void init_gpio(void) {
  // Configure LED
  P1DIR |= LED_PIN; // Set LED pin as output
  LED_OFF();        // Initially turn LED off
}

/**
 * @brief Initialize SPI and other peripherals
 */
void init_peripherals(void) {
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
