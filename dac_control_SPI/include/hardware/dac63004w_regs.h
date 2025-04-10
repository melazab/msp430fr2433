/**
 * @file dac63004w_regs.h
 * @brief Register definitions for the DAC63004W
 */
#ifndef DAC63004W_REGS_H
#define DAC63004W_REGS_H

#include <stdint.h>

#define DAC_REG_X_DATA             0x19 
#define DAC_REG_X_VOUT_CONFIG      0x03
#define DAC_REG_X_IOUT_MISC_CONFIG 0x04

#define DAC_REG_COMMON_CONFIG   0x1F    // Common configuration register
#define DAC_REG_COMMON_TRIGGER  0x20    // Common trigger register
#define DAC_REG_COMMON_DAC_TRIG 0x21    // Common DAC trig register
#define DAC_REG_DEVICE_MODE     0x25    // Device mode configuration
#define DAC_REG_INTERFACE       0x26    // Interface configuration

/**
 * @brief COMMON-DAC-TRIG register bits
 */
#define START_FUNC_GEN    1
#define STOP_FUNC_GEN     0


#define DAC_VOUT_GAIN_1X_EXT_REFERENCE        (0 << 10)
#define DAC_VOUT_GAIN_1X_VDD_REFERENCE        (1 << 10)
#define DAC_VOUT_GAIN_1P5X_INT_REFERENCE      (2 << 10)
#define DAC_VOUT_GAIN_2X_INT_REFERENCE        (3 << 10)
#define DAC_VOUT_GAIN_3X_INT_REFERENCE        (4 << 10)
#define DAC_VOUT_GAIN_4X_INT_REFERENCE        (5 << 10)

/**
 * @brief DAC-X-FUNC-CONFIG registers
 */
#define DAC_REG_DAC0_FUNC_CONFIG    0x06
#define DAC_REG_DAC1_FUNC_CONFIG    0x0C
#define DAC_REG_DAC2_FUNC_CONFIG    0x12
#define DAC_REG_DAC3_FUNC_CONFIG    0x18

/**
 * @brief FUNC-CONFIG-X register bits
 */
#define DAC_FUNC_CONFIG_TRIANGLE            (0 << 8)  // triangle wave
#define DAC_FUNC_CONFIG_SAWTOOTH            (1 << 8)  // sawtooth wave
#define DAC_FUNC_CONFIG_SAWTOOTH_INV        (2 << 8)  // inverted sawtooth wave
#define DAC_FUNC_CONFIG_SINE                (4 << 8)  // sine wave
#define DAC_FUNC_CONFIG_DISABLE             (7 << 8)  // Disable function generation

#define DAC_PHASE_SEL_0                     (0 << 11)  // 0° Phase
#define DAC_PHASE_SEL_120                   (1 << 11)  // 120° Phase
#define DAC_PHASE_SEL_240                   (2 << 11)  // 240° Phase
#define DAC_PHASE_SEL_90                    (3 << 11)  // 90° Phase

/**
 * @brief Common Config register bits
 */
#define DAC_EN_INT_REF         (1 << 12)   // Enable internal reference
#define DAC_VOUT_PDN_MASK      (0x3 << 10) // Voltage output power down mask
#define DAC_VOUT_PDN_NORMAL    0x00        // Normal operation
#define DAC_VOUT_PDN_HIZ       0x03        // Hi-Z to AGND

/**
 * @brief Common Trigger register bits
 */
#define DAC_RESET_TRIGGER           (0xA << 8)// Reset trigger value
#define DAC_LDAC_TRIGGER            (1 << 7)  // LDAC trigger bit
#define DAC_CLR_TRIGGER             (1 << 6)  // Clear trigger bit
#define DAC_START_FUNC_0            (1 << 0)  // Start function for channel 0
#define DAC_START_FUNC_1            (1 << 4)  // Start function for channel 1
#define DAC_START_FUNC_2            (1 << 8)  // Start function for channel 2
#define DAC_START_FUNC_3            (1 << 12) // Start function for channel 3

/**
 * @brief Device mode config bits
 */
#define DAC_MODE_NORMAL        0x00        // Normal operation mode
#define DAC_MODE_LOWPOWER      (1 << 13)   // Low power mode

/**
 * @brief Helper macros for DAC data formatting
 */
#define DAC_DATA_12BIT(val)    ((val & 0xFFF) << 4)  // 12-bit data left aligned
#define DAC_DATA_MIDSCALE      0x8000               // Mid-scale value

/**
 * @brief Slew rate definitions for wave generation
 */
typedef enum {
    DAC_SLEW_RATE_NONE = 0x0,        // No slew (invalid for waveform generation)
    DAC_SLEW_RATE_4_US = 0x1,       // 4 µs/step
    DAC_SLEW_RATE_8_US = 0x2,       // 8 µs/step
    DAC_SLEW_RATE_12_US = 0x3,      // 12 µs/step
    DAC_SLEW_RATE_18_US = 0x4,      // 18 µs/step
    DAC_SLEW_RATE_27_US = 0x5,      // 27.04 µs/step
    DAC_SLEW_RATE_40_US = 0x6,      // 40.48 µs/step
    DAC_SLEW_RATE_61_US = 0x7,      // 60.72 µs/step
    DAC_SLEW_RATE_91_US = 0x8,      // 91.12 µs/step
    DAC_SLEW_RATE_137_US = 0x9,     // 136.72 µs/step
    DAC_SLEW_RATE_239_US = 0xA,     // 239.2 µs/step
    DAC_SLEW_RATE_418_US = 0xB,     // 418.64 µs/step
    DAC_SLEW_RATE_733_US = 0xC,     // 732.56 µs/step
    DAC_SLEW_RATE_1282_US = 0xD,    // 1282 µs/step
    DAC_SLEW_RATE_2564_US = 0xE,    // 2563.96 µs/step
    DAC_SLEW_RATE_5128_US = 0xF     // 5127.92 µs/step
} slew_rate_t;

#endif /* DAC63004W_REGS_H */
