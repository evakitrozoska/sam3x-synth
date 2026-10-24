#ifndef LFO_H_INCLUDED
#define LFO_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <config.h>
#include <wavetable.h>

typedef enum {
 FREQ, // not implemented yet
 AMPL,
 OFF
}lfo_mode_t;

typedef struct{
	uint32_t freq;
	uint32_t phase;
	uint32_t phase_incr;
	uint32_t depth;
	const int16_t *wave;
	bool state;
	lfo_mode_t mode;
}lfo_t;

uint16_t lfo_next_sample(void);
void set_lfo_freq(uint32_t freq);

#endif // LFO_H_INCLUDED
