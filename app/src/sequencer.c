#include <sequencer.h>

/*
 * Step sequencer driven by a hardware timer.
 * The timer requests note changes, while the main loop updates
 * the oscillator frequency and triggers the ADSR envelope.
 */

void seq_start(seq_t *seq){
    if(seq->steps == 0) return;
    seq->seq_on = 1;
    seq->seq_idx = 0;
    seq->note_change = 1;
}

void seq_stop(seq_t *seq){
    seq->seq_on = 0;
    seq->note_change = 0;
}

void seq_process(seq_t *seq, oscillator_t *osc, adsr_t *adsr){
/*
 * Process pending note changes in the main loop.
 */
    if(!seq->note_change || !seq->seq_on) return;
    seq->note_change = 0;
    uint32_t freq = seq->sequence[seq->seq_idx];
    set_freq(freq, osc);
    if(freq!=0){
        adsr_trigger(adsr);
    }
    seq->seq_idx++;
    if (seq->seq_idx >= seq->steps){
        seq->seq_idx = 0;
    }
}
