#ifndef SEQUENCER_H_INCLUDED
#define SEQUENCER_H_INCLUDED

#include <oscillator.h>
#include <adsr.h>
#include <stdbool.h>

#define MAX_STEPS 70

typedef struct{
    volatile bool seq_on;
    volatile bool note_change;

    volatile uint16_t seq_idx;
    volatile uint16_t steps;

    uint32_t step_ms; // Duration of one sequencer step (ms)
    uint32_t ms_count;
    uint32_t sequence[MAX_STEPS];
}seq_t;

void seq_start(seq_t *seq);
void seq_stop(seq_t *seq);
void seq_process(seq_t *seq, oscillator_t *osc, adsr_t *adsr);

#endif // SEQUENCER_H_INCLUDED
