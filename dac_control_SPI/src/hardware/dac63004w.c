#include "hardware/dac63004w.h"
#include "hardware/msp_spi.h"
#include <stddef.h> /* For NULL definition */
#include <stdio.h>

static uint8_t dac_get_data_register(uint8_t channel);
static uint8_t dac_get_vout_config_register(uint8_t channel);
static uint8_t dac_get_iout_config_register(uint8_t channel);
static uint8_t dac_get_iout_config_register(uint8_t channel);

static uint8_t dac_get_data_register(uint8_t channel) {
  uint8_t base = DAC_REG_X_DATA;
  if (channel > 3) {
    return 0xFF; // Invalid channel
  }
  return base + channel;
}

static uint8_t dac_get_vout_config_register(uint8_t channel) {
  uint8_t base = DAC_REG_X_VOUT_CONFIG;
  if (channel > 3) {
    return 0xFF; // Invalid channel
  }
  return base + (channel * 6);
}

static uint8_t dac_get_iout_config_register(uint8_t channel) {
  uint8_t base = DAC_REG_X_IOUT_MISC_CONFIG;
  if (channel > 3) {
    return 0xFF; // Invalid channel
  }
  return base + (channel * 6);
}

static dac63004w_status_t dac_write_register(uint8_t reg, uint16_t value) {
  if (reg == 0xFF) {
    return DAC_ERROR_PARAM;
  }

  printf("Writing to DAC register 0x%02X with value 0x%04X\n", reg, value);

  uint8_t tx_data[3];
  tx_data[0] = (reg & 0x7F);        // Register address (7 bits) + R/W=0
  tx_data[1] = (value >> 8) & 0xFF; // Data MSB
  tx_data[2] = value & 0xFF;        // Data LSB

  // Perform SPI transfer
  int status = msp_spi_transfer(tx_data, NULL, sizeof(tx_data));
  if (status != SPI_SUCCESS) {
    printf("SPI write error: %d\n", status);
    return DAC_ERROR_COMM;
  }

  return DAC_SUCCESS;
}

dac63004w_status_t dac_reset(void) {
  return dac_write_register(DAC_REG_COMMON_TRIGGER, 0x0A00);
}

dac63004w_status_t dac_trigger_ldac(void) {
  return dac_write_register(DAC_REG_COMMON_TRIGGER, 0x0080); // DAC_LDAC_TRIGGER
}
dac63004w_status_t dac_init(dac63004w_context_t *ctx) {
  if (!ctx) {
    return DAC_ERROR_PARAM;
  }

  // Step 1: Software reset
  if (dac_reset() != DAC_SUCCESS) {
    printf("DAC reset failed\n");
  }

  usleep(1000); // Wait for reset to complete

  // Step 2: Configure gain settings for each channel
  uint16_t gain_config =
      DAC_VOUT_GAIN_1P5X_INT_REFERENCE; // Using internal reference
  for (uint8_t channel = 0; channel < 4; channel++) {
    uint8_t reg_addr = dac_get_vout_config_register(channel);
    if (dac_write_register(reg_addr, gain_config) != DAC_SUCCESS) {
      printf("Failed to configure gain settings for channel %d\n", channel);
      return DAC_ERROR_COMM;
    }
  }

  // Step 5: Enable internal reference and set normal operation
  status = dac_write_register(DAC_REG_COMMON_CONFIG, 0x1249);
  if (status != DAC_SUCCESS) {
    return status;
  }

  // Step 6: Initialize all DAC channels to 0V
  for (uint8_t channel = 0; channel < 4; channel++) {
    uint8_t reg_addr = dac_get_data_register(channel);
    status = dac_write_register(reg_addr, 0x0000);
    if (status != DAC_SUCCESS) {
      return status;
    }
  }

  // Step 7: Trigger LDAC to update all outputs
  status = dac_trigger_ldac();
  if (status != DAC_SUCCESS) {
    return status;
  }

  // Wait for settings to take effect
  usleep(1000);
  return DAC_SUCCESS;
}

dac63004w_status_t dac_set_mode(dac63004w_context_t *ctx, dac_mode_t mode) {
  if (!ctx) {
    ctx = &dac_ctx;
  }

  ctx->mode = mode;
  dac_ctx.mode = mode;

  return DAC_SUCCESS;
}

dac63004w_status_t dac_configure_voltage_mode(dac63004w_context_t *ctx,
                                              uint8_t channel, uint16_t gain) {
  if (channel > 3) {
    return DAC_ERROR_PARAM;
  }

  if (!ctx) {
    ctx = &dac_ctx;
  }

  // Configure the VOUT_CMP_CONFIG register for the channel
  uint8_t reg_addr = dac_get_vout_config_register(channel);

  dac_status_t status = dac_write_register(reg_addr, gain);
  if (status != DAC_SUCCESS) {
    return status;
  }

  // Update the operating mode
  ctx->mode = DAC_MODE_VOLTAGE;
  dac_ctx.mode = DAC_MODE_VOLTAGE;

  return DAC_SUCCESS;
}

/**
 * @brief Configure a channel for current output mode
 *
 * @param ctx DAC context
 * @param channel DAC channel (0-3)
 * @return dac_status_t Status code
 */
dac_status_t dac_configure_current_mode(dac_context_t *ctx, uint8_t channel) {
  if (channel > 3) {
    return DAC_ERROR_PARAM;
  }

  if (!ctx) {
    ctx = &dac_ctx;
  }

  // Configure IOUT_MISC_CONFIG for ±250 µA range
  uint8_t iout_config_reg = dac_get_iout_config_register(channel);

  // 1011b = -250 µA to +250 µA range
  uint16_t iout_config = (0xB << 9);

  dac_status_t status = dac_write_register(iout_config_reg, iout_config);
  if (status != DAC_SUCCESS) {
    return status;
  }

  // Configure COMMON_CONFIG for current mode
  // - Enable internal reference
  // - Set VOUT to Hi-Z
  // - Power up IOUT
  uint16_t common_config = 0x1DB6;
  status = dac_write_register(DAC_REG_COMMON_CONFIG, common_config);
  if (status != DAC_SUCCESS) {
    return status;
  }

  // Update the operating mode
  ctx->mode = DAC_MODE_CURRENT;
  dac_ctx.mode = DAC_MODE_CURRENT;

  return DAC_SUCCESS;
}

/**
 * @brief Set a DC voltage output on a DAC channel
 *
 * @param ctx DAC context
 * @param channel DAC channel (0-3)
 * @param voltage Voltage value (V)
 * @return dac_status_t Status code
 */
dac_status_t dac_write_voltage(dac_context_t *ctx, uint8_t channel,
                               float voltage) {
  if (channel > 3) {
    return DAC_ERROR_PARAM;
  }

  if (!ctx) {
    ctx = &dac_ctx;
  }

  // Validate voltage range
  if (voltage < 0 || voltage > ctx->vref) {
    return DAC_ERROR_PARAM;
  }

  // If in current mode, switch to voltage mode
  if (ctx->mode != DAC_MODE_VOLTAGE) {
    dac_status_t status = dac_configure_voltage_mode(
        ctx, channel, DAC_VOUT_GAIN_1X_VDD_REFERENCE);
    if (status != DAC_SUCCESS) {
      return status;
    }
  }

  // Calculate DAC code - 12-bit value left-aligned in 16-bit word
  uint16_t dac_code = (uint16_t)(((voltage / ctx->vref) * 0xFFF) + 0.5) << 4;

  // Get data register for the channel
  uint8_t reg_addr = dac_get_data_register(channel);

  // Write to DAC register
  dac_status_t status = dac_write_register(reg_addr, dac_code);
  if (status != DAC_SUCCESS) {
    return status;
  }

  // Trigger LDAC to update outputs
  return dac_trigger_ldac();
}

/**
 * @brief Set a DC current output on a DAC channel
 *
 * @param ctx DAC context
 * @param channel DAC channel (0-3)
 * @param current_ua Current value (μA)
 * @return dac_status_t Status code
 */
dac_status_t dac_write_current(dac_context_t *ctx, uint8_t channel,
                               int current_ua) {
  if (channel > 3) {
    return DAC_ERROR_PARAM;
  }

  if (!ctx) {
    ctx = &dac_ctx;
  }

  // Validate current range (±250 μA)
  if (current_ua < -250 || current_ua > 250) {
    return DAC_ERROR_PARAM;
  }

  // If in voltage mode, switch to current mode
  if (ctx->mode != DAC_MODE_CURRENT) {
    dac_status_t status = dac_configure_current_mode(ctx, channel);
    if (status != DAC_SUCCESS) {
      return status;
    }
  }

  // Integer-based DAC code calculation
  // Maps -250 to +250 µA to 0x0000 to 0x0FFF
  uint16_t dac_code = (uint16_t)(((current_ua + 250) * 4095) / 500);

  // Left-align DAC code
  dac_code <<= 4;

  // Get data register for the channel
  uint8_t reg_addr = dac_get_data_register(channel);

  // Write to DAC register
  dac_status_t status = dac_write_register(reg_addr, dac_code);
  if (status != DAC_SUCCESS) {
    return status;
  }

  // Trigger LDAC to update outputs
  return dac_trigger_ldac();
}
