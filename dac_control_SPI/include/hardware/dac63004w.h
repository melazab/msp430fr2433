#ifndef DAC63004W_H
#define DAC63004W_H

#include <stdint.h>
#include "hardware/dac63004w_regs.h"

typedef enum {
    DAC_SUCCESS = 0,
    DAC_ERROR_INIT = -1,
    DAC_ERROR_PARAM = -2,
    DAC_ERROR_COMM = -3
} dac63004w_status_t;

typedef enum {
    DAC_MODE_VOLTAGE = 0,
    DAC_MODE_CURRENT = 1
} dac63004w_mode_t;

/**
 * @brief DAC device context
 */
typedef struct {
    float vref;           // Reference voltage (V)
    dac63004w_mode_t mode;      // Operating mode (voltage or current)
    uint8_t channel;
} dac63004w_context_t;

dac63004w_status_t dac_init(dac_context_t *ctx)
dac63004w_status_t dac_write_voltage(dac_context_t *ctx, uint8_t channel, float voltage);
dac63004w_status_t dac_write_current(dac_context_t *ctx, uint8_t channel, int current_ua);
dac63004w_status_t dac_configure_voltage_mode(dac_context_t *ctx, uint8_t channel, uint16_t gain);
dac63004w_status_t dac_configure_current_mode(dac_context_t *ctx, uint8_t channel);
dac63004w_status_t dac_set_mode(dac_context_t *ctx, dac63004w_mode_t mode);
dac63004w_status_t dac_trigger_ldac(void);
dac63004w_status_t dac_reset(void);

#endif /* DAC63004W_H */
