#include <oscillator.h>
/*
 * DDS oscillator using additive synthesis.
 */

int16_t osc_next_sample(oscillator_t *osc){
/*
 * Generate one output sample.
 * LUT range: -2048...2047
 */
    int32_t ret=0;
    for(uint32_t i=0; i<osc->harmonics; i++){
        osc->phase[i]+=osc->phase_incr*(i+1);
        uint16_t idx = osc->phase[i] >> (PHASE_BITS-LUT_BITS); // most significant phase bits as the wavetable index
        ret = ret + ((osc->ampl[i]*(osc->wave[idx]))>>AMP_SHIFT);  // normalize
    }
    return (int16_t)ret;
}

void set_freq(uint32_t freq, oscillator_t *osc){
/*
 * Update the DDS phase increment.
 */
    osc->freq = freq;
    osc->phase_incr = (uint32_t)(((uint64_t)(freq)<<PHASE_BITS)/FS);
}
