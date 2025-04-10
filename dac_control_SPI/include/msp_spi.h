/**
 * @file msp_spi.h
 * @brief MSP430 SPI interface implementation
 */
#ifndef MSP_SPI_H
#define MSP_SPI_H

#include <stdint.h>

/**
 * @brief SPI operation status codes
 */
typedef enum {
    SPI_SUCCESS = 0,
    SPI_ERROR_INIT = -1,
    SPI_ERROR_TRANSFER = -2,
    SPI_ERROR_PARAM = -3
} spi_status_t;

/**
 * @brief SPI pin configuration structure
 */
typedef struct {
    uint8_t mosi_port;
    uint8_t mosi_pin;
    uint8_t miso_port;
    uint8_t miso_pin;
    uint8_t sclk_port;
    uint8_t sclk_pin;
    uint8_t cs_port;
    uint8_t cs_pin;
} spi_pin_config_t;

/**
 * @brief SPI configuration options
 */
typedef struct {
    uint8_t clock_divider;  // SPI clock divider from SMCLK
    uint8_t mode;           // SPI mode (0-3)
    uint8_t bit_order;      // Bit order (MSB or LSB first)
} spi_config_t;

/**
 * @brief Initialize SPI hardware
 * 
 * @param pins Pin configuration structure
 * @param config SPI configuration options
 * @return spi_status_t Status code
 */
spi_status_t msp_spi_init(const spi_pin_config_t *pins, const spi_config_t *config);

/**
 * @brief Send a single byte over SPI
 * 
 * @param data Byte to send
 * @return uint8_t Received byte (if applicable)
 */
uint8_t msp_spi_transfer_byte(uint8_t data);

/**
 * @brief Send multiple bytes over SPI with CS control
 * 
 * @param tx_data Transmit buffer
 * @param rx_data Receive buffer (can be NULL for write-only)
 * @param length Number of bytes to transfer
 * @return spi_status_t Status code
 */
spi_status_t msp_spi_transfer(const uint8_t *tx_data, uint8_t *rx_data, uint16_t length);

/**
 * @brief Assert (lower) the CS pin
 */
void msp_spi_cs_assert(void);

/**
 * @brief Deassert (raise) the CS pin
 */
void msp_spi_cs_deassert(void);

/**
 * @brief Deinitialize SPI hardware
 * 
 * @return spi_status_t Status code
 */
spi_status_t msp_spi_deinit(void);

#endif /* MSP_SPI_H */
