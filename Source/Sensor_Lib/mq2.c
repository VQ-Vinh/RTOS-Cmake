/**
 * @file mq2.c
 * @brief MQ2 Gas Sensor Library Implementation
 * Uses Driver/adc.c for ADC operations
 *
 * TODO: Expand for PPM calculation (sensitivity curve)
 */

#include "mq2.h"
#include "../Driver/adc.h"

/* Static variable to track initialization */
static uint8_t s_initialized = 0;

/* ADC channel configuration for MQ2 */
static ADC_Channel_t mq2_adc_channel[] = {
    {MQ2_ADC_CHANNEL, MQ2_ADC_SAMPLE}
};

void mq2Init(void) {
    /* Initialize ADC with MQ2 channel configuration */
    adcInit(mq2_adc_channel, 1);
    s_initialized = 1;
}

uint16_t mq2ReadADC(void) {
    if (!s_initialized) {
        return 0;
    }
    /* Read channel index 0 = MQ2_ADC_CHANNEL */
    return adcRead(0);
}

uint16_t mq2ReadPPM(void) {
    /* TODO: Implement PPM calculation
     * MQ2 sensitivity curve for LPG, propane, hydrogen, methane, etc.
     * Requires calibration with known gas concentrations
     *
     * Basic approach:
     * 1. Read sensor resistance: Rs = (Vc - Vout) / Vout * RL
     * 2. Calculate ratio: ratio = Rs / Ro (Ro = clean air resistance)
     * 3. Apply power curve: PPM = a * (ratio^b)
     */
    return mq2ReadADC();  /* For now, return raw ADC value */
}

uint8_t mq2IsInitialized(void) {
    return s_initialized;
}
