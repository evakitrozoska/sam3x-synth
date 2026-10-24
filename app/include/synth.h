#ifndef SYNTH_H_INCLUDED
#define SYNTH_H_INCLUDED

#include <stdbool.h>
#include <oscillator.h>
#include <adsr.h>
#include <lfo.h>
#include <sequencer.h>
#include <wavetable.h>
#include <tc.h>
#include <sysclk.h>
#include <config.h>

#define MAX_VOICES 4

typedef struct voice{
    oscillator_t osc;
    adsr_t adsr;
    seq_t seq;
}voice_t;

typedef enum {
    WAVETABLE = 0x00,
    ADDITIVE = 0x01
}synth_mode_t;

uint16_t synth_next_sample(void);
void synth_process(void);
void seq_timer_setup(void);
void synth_start(void);
void synth_stop(void);

#endif // SYNTH_H_INCLUDED
