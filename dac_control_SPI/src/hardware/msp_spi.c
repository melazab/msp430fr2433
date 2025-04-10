/**
 * @file msp_spi.c
 * @brief MSP430 SPI interface implementation
 */
#include "hardware/msp_spi.h"
#include <msp430.h>
#include <stddef.h> /* For NULL definition */

// SPI pin configuration globals
static uint8_t g_cs_port;
static uint8_t g_cs_pin;
static uint8_t g_cs_pin_mask;

// SPI mode parameters
#define SPI_MODE_0 0x00 // CPOL=0, CPHA=0
#define SPI_MODE_1 0x01 // CPOL=0, CPHA=1
#define SPI_MODE_2 0x02 // CPOL=1, CPHA=0
#define SPI_MODE_3 0x03 // CPOL=1, CPHA=1

// Bit order parameters
#define SPI_MSB_FIRST 0
#define SPI_LSB_FIRST 1

/**
 * @brief Configure GPIO for SPI operation
 *
 * @param pins SPI pin configuration
 * @return spi_status_t Status code
 */
static spi_status_t configure_gpio(const spi_pin_config_t *pins) {
  if (!pins) {
    return SPI_ERROR_PARAM;
  }

  // Store CS pin info for later use
  g_cs_port = pins->cs_port;
  g_cs_pin = pins->cs_pin;
  g_cs_pin_mask = (1 << pins->cs_pin);

  // Configure CS pin as GPIO output
  switch (g_cs_port) {
  case 1:
    P1SEL0 &= ~g_cs_pin_mask;
    P1SEL1 &= ~g_cs_pin_mask;
    P1DIR |= g_cs_pin_mask;
    P1OUT |= g_cs_pin_mask; // Initialize CS high (inactive)
    break;
  case 2:
    P2SEL0 &= ~g_cs_pin_mask;
    P2SEL1 &= ~g_cs_pin_mask;
    P2DIR |= g_cs_pin_mask;
    P2OUT |= g_cs_pin_mask; // Initialize CS high (inactive)
    break;
  default:
    return SPI_ERROR_PARAM;
  }

  // Configure SPI pins based on the port number
  if (pins->mosi_port == 1 && pins->sclk_port == 1) {
    // Configure P1.4 (MOSI) and P1.6 (SCK) for SPI
    P1SEL0 |= (1 << pins->mosi_pin) | (1 << pins->sclk_pin);
    P1SEL1 &= ~((1 << pins->mosi_pin) | (1 << pins->sclk_pin));

    // Configure MISO if needed (for full duplex)
    if (pins->miso_port == 1) {
      P1SEL0 |= (1 << pins->miso_pin);
      P1SEL1 &= ~(1 << pins->miso_pin);
    }
  } else {
    return SPI_ERROR_PARAM;
  }

  return SPI_SUCCESS;
}

/**
 * @brief Configure SPI module settings
 *
 * @param config SPI configuration options
 * @return spi_status_t Status code
 */
static spi_status_t configure_spi(const spi_config_t *config) {
  if (!config) {
    return SPI_ERROR_PARAM;
  }

  // Put SPI module in reset state
  UCA0CTLW0 = UCSWRST;

  // Configure as SPI master with specified settings
  UCA0CTLW0 |= UCMST | UCSYNC; // SPI master, synchronous mode

  // Configure clock source
  UCA0CTLW0 |= UCSSEL__SMCLK; // Use SMCLK as clock source

  // Configure SPI mode (clock polarity and phase)
  switch (config->mode) {
  case SPI_MODE_0:
    UCA0CTLW0 &= ~(UCCKPH | UCCKPL);
    break;
  case SPI_MODE_1:
    UCA0CTLW0 |= UCCKPH;
    UCA0CTLW0 &= ~UCCKPL;
    break;
  case SPI_MODE_2:
    UCA0CTLW0 &= ~UCCKPH;
    UCA0CTLW0 |= UCCKPL;
    break;
  case SPI_MODE_3:
    UCA0CTLW0 |= (UCCKPH | UCCKPL);
    break;
  default:
    return SPI_ERROR_PARAM;
  }

  // Configure bit order
  if (config->bit_order == SPI_MSB_FIRST) {
    UCA0CTLW0 |= UCMSB; // MSB first (most common)
  } else {
    UCA0CTLW0 &= ~UCMSB; // LSB first
  }

  // Configure clock divider
  UCA0BR0 = config->clock_divider;
  UCA0BR1 = 0;
  UCA0MCTLW = 0; // No modulation for SPI

  // Enable SPI
  UCA0CTLW0 &= ~UCSWRST;

  return SPI_SUCCESS;
}

/**
 * @brief Initialize SPI hardware
 *
 * @param pins Pin configuration structure
 * @param config SPI configuration options
 * @return spi_status_t Status code
 */
spi_status_t msp_spi_init(const spi_pin_config_t *pins,
                          const spi_config_t *config) {
  // Configure GPIO pins
  spi_status_t status = configure_gpio(pins);
  if (status != SPI_SUCCESS) {
    return status;
  }

  // Configure SPI module
  status = configure_spi(config);
  if (status != SPI_SUCCESS) {
    return status;
  }

  return SPI_SUCCESS;
}

/**
 * @brief Assert (lower) the CS pin
 */
void msp_spi_cs_assert(void) {
  // Lower CS pin to activate device
  switch (g_cs_port) {
  case 1:
    P1OUT &= ~g_cs_pin_mask;
    break;
  case 2:
    P2OUT &= ~g_cs_pin_mask;
    break;
  }

  // Small delay to ensure CS is stable
  __delay_cycles(10);
}

/**
 * @brief Deassert (raise) the CS pin
 */
void msp_spi_cs_deassert(void) {
  // Small delay to ensure last transfer is complete
  __delay_cycles(10);

  // Raise CS pin to deactivate device
  switch (g_cs_port) {
  case 1:
    P1OUT |= g_cs_pin_mask;
    break;
  case 2:
    P2OUT |= g_cs_pin_mask;
    break;
  }

  // Small delay after CS change
  __delay_cycles(10);
}

/**
 * @brief Send a single byte over SPI
 *
 * @param data Byte to send
 * @return uint8_t Received byte (if applicable)
 */
uint8_t msp_spi_transfer_byte(uint8_t data) {
  while (!(UCA0IFG & UCTXIFG))
    ;               // Wait for TX buffer to be ready
  UCA0TXBUF = data; // Send byte
  while (!(UCA0IFG & UCRXIFG))
    ;               // Wait for RX to complete
  return UCA0RXBUF; // Return received byte
}

/**
 * @brief Send multiple bytes over SPI with CS control
 *
 * @param tx_data Transmit buffer
 * @param rx_data Receive buffer (can be NULL for write-only)
 * @param length Number of bytes to transfer
 * @return spi_status_t Status code
 */
spi_status_t msp_spi_transfer(const uint8_t *tx_data, uint8_t *rx_data,
                              uint16_t length) {
  if (!tx_data || length == 0) {
    return SPI_ERROR_PARAM;
  }

  // Assert CS
  msp_spi_cs_assert();

  // Transfer each byte
  for (uint16_t i = 0; i < length; i++) {
    uint8_t rx_byte = msp_spi_transfer_byte(tx_data[i]);

    // Store received byte if rx_data is not NULL
    if (rx_data) {
      rx_data[i] = rx_byte;
    }
  }

  // Deassert CS
  msp_spi_cs_deassert();

  return SPI_SUCCESS;
}

/**
 * @brief Deinitialize SPI hardware
 *
 * @return spi_status_t Status code
 */
spi_status_t msp_spi_deinit(void) {
  // Put SPI in reset state
  UCA0CTLW0 = UCSWRST;

  return SPI_SUCCESS;
}
