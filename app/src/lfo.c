#include <lfo.h>

lfo_t lfo = {
    .freq = 100,
    .phase = 0,
    .phase_incr = 10226126,
    .wave = sine,
    .depth = 30000, // 0-65535
    .state = 1,
    .mode = OFF
};

uint16_t lfo_next_sample(void){
	/*
	 * Generate one LFO sample for amplitude modulation.
	 *
	 * The LUT contains a bipolar waveform (-2048...2047), which is
	 * converted to a unipolar gain value (0...65520).
	 *
	 * The depth parameter controls the modulation amount:
	 *   depth = 0      -> no modulation, output gain = 65535
	 *   depth = 65535  -> full modulation, output gain follows LFO (0...65535)
	 *
	 * Return value:
	 *   Q16 amplitude gain (0...65535), used to scale the audio sample.
	 */
	lfo.phase += lfo.phase_incr;
	uint16_t idx = lfo.phase >> (PHASE_BITS - LUT_BITS);
	int32_t sample = lfo.wave[idx]; // LUT: -2048 ... +2047
	sample += DAC_OFFSET; // 0 ... 4095
	uint32_t lfo_value = (uint32_t)sample << 4; // 0 ... 65520
	uint32_t gain = 65535 - lfo.depth + (((uint32_t)lfo.depth * lfo_value) >> 16);
	return (uint16_t)gain;
}

void set_lfo_freq(uint32_t freq){
    lfo.freq = freq;
    lfo.phase_incr = (uint32_t)(((uint64_t)(freq)<<PHASE_BITS)/FS);
}
