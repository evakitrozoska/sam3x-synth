#ifndef OSCILLATOR_H_INCLUDED
#define OSCILLATOR_H_INCLUDED

#include <stdint.h>
#include <wavetable.h>
#include <stdbool.h>
#include <config.h>

#define MAX_HARMONICS 14

typedef struct{
    uint32_t freq;
    uint32_t harmonics;
    uint32_t phase[MAX_HARMONICS];
    uint32_t phase_incr;
    uint16_t ampl[MAX_HARMONICS];
    const int16_t *wave;
    bool state;
}oscillator_t;

int16_t osc_next_sample(oscillator_t *osc);
void set_freq(uint32_t freq, oscillator_t *osc);

#endif // OSCILLATOR_H_INCLUDED
