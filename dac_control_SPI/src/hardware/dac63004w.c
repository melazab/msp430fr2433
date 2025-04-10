#include "hardware/dac63004w.h"
#include "msp_delay.h"
#include "msp_spi.h"
#include <stddef.h> /* For NULL definition */

static uint8_t dac_get_data_register(uint8_t channel);
static uint8_t dac_get_vout_config_register(uint8_t channel);

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

static dac63004w_status_t dac_write_register(uint8_t reg, uint16_t value) {
  if (reg == 0xFF) {
    return DAC_ERROR_PARAM;
  }

  uint8_t tx_data[3];
  tx_data[0] = (reg & 0x7F);        // Register address (7 bits) + R/W=0
  tx_data[1] = (value >> 8) & 0xFF; // Data MSB
  tx_data[2] = value & 0xFF;        // Data LSB

  // Perform SPI transfer
  int status = msp_spi_transfer(tx_data, NULL, sizeof(tx_data));
  if (status != SPI_SUCCESS) {
    return DAC_ERROR_COMM;
  }

  return DAC_SUCCESS;
}

dac63004w_status_t dac_reset(void) {
  return dac_write_register(DAC_REG_COMMON_TRIGGER, 0x0A00); // Power-on reset
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
  }

  delay_ms(1);

  // Step 2: Configure gain settings for each channel
  uint16_t gain_config =
      DAC_VOUT_GAIN_1P5X_INT_REFERENCE; // Using internal reference
  for (uint8_t channel = 0; channel < 4; channel++) {
    uint8_t reg_addr = dac_get_vout_config_register(channel);
    if (dac_write_register(reg_addr, gain_config) != DAC_SUCCESS) {
      return DAC_ERROR_COMM;
    }
  }

  // Step 3: Enable internal reference and set normal operation
  if (dac_write_register(DAC_REG_COMMON_CONFIG, 0x1249) != DAC_SUCCESS) {
    return DAC_ERROR_COMM;
  }

  // Step 4: Trigger LDAC to update all outputs
  if (dac_trigger_ldac() != DAC_SUCCESS) {
    return DAC_ERROR_COMM;
  }

  // Wait for settings to take effect
  /* usleep(1000); */
  return DAC_SUCCESS;
}

dac63004w_status_t dac_set_mode(dac63004w_context_t *ctx,
                                dac63004w_mode_t mode) {
  // TODO: Add error handling to check proper mode parameter was entered
  if (!ctx) {
    return DAC_ERROR_PARAM;
  }

  ctx->mode = mode;

  return DAC_SUCCESS;
}

dac63004w_status_t dac_configure_voltage_mode(dac63004w_context_t *ctx,
                                              uint16_t gain, uint8_t channel) {
  if (!ctx || channel > 3) {
    return DAC_ERROR_PARAM;
  }

  // Configure the VOUT_CMP_CONFIG register for the channel
  uint8_t reg_addr = dac_get_vout_config_register(channel);

  if (dac_write_register(reg_addr, gain) != DAC_SUCCESS) {
    return DAC_ERROR_PARAM;
  }

  // Update the operating mode
  ctx->mode = DAC_MODE_VOLTAGE;

  return DAC_SUCCESS;
}

dac63004w_status_t dac_write_voltage(dac63004w_context_t *ctx, uint8_t channel,
                                     float voltage) {
  if (!ctx || channel > 3) {
    return DAC_ERROR_PARAM;
  }

  // Validate voltage range
  if (voltage < 0 || voltage > ctx->vref) {
    return DAC_ERROR_PARAM;
  }

  // If in current mode, switch to voltage mode
  if (ctx->mode != DAC_MODE_VOLTAGE) {
    dac63004w_status_t status = dac_configure_voltage_mode(
        ctx, DAC_VOUT_GAIN_1X_VDD_REFERENCE, channel);
    if (status != DAC_SUCCESS) {
      return status;
    }
  }

  // Calculate DAC code - 12-bit value left-aligned in 16-bit word
  uint16_t dac_code = (uint16_t)(((voltage / ctx->vref) * 0xFFF) + 0.5) << 4;

  // Get data register for the channel
  uint8_t reg_addr = dac_get_data_register(channel);

  // Write to DAC register
  dac63004w_status_t status = dac_write_register(reg_addr, dac_code);
  if (status != DAC_SUCCESS) {
    return status;
  }

  // Trigger LDAC to update outputs
  return dac_trigger_ldac();
}
