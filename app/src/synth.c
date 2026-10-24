#include <synth.h>

extern lfo_t lfo;

synth_mode_t synth_mode = WAVETABLE;
uint32_t synth_active_voices = 2;

uint32_t step_rc = 0;

voice_t wavetable_voices[MAX_VOICES] = {
    // =========================================================================
    // Voice 0 - Melody
    // =========================================================================
    {
        .osc = {
            .freq = 659,
            .harmonics = 1,
            .phase = {0},
            .phase_incr = 0,
            .ampl = {400},
            .wave = saw,
            .state = true
        },

        .adsr = {
            .attack_percent = 10,
            .decay_percent = 25,
            .sustain_level = 30000,
            .release_percent = 20
        },

        .seq = {
            .seq_on = 1,
            .note_change = 0,
            .seq_idx = 0,
            .steps = 64,
            .step_ms = 200,
            .ms_count = 0,
            .sequence = {
                659,0,
                494,
                523,
                587,0,

                523,
                494,
                440,0,

                440,
                523,
                659,0,

                587,
                523,
                494,0,

                0,
                523,
                587,0,
                659,0,

                523,0,
                440,0,

                440,0,0,0,

                0,
                587,0,

                698,
                880,0,

                784,
                698,

                659,0,

                0,
                523,
                659,0,

                587,
                523,

                494,0,

                494,
                523,
                587,0,

                659,0,

                523,0,
                440,0,

                440,0,0,0
            }
        }
    },

    // =========================================================================
    // Voice 1 - Harmony
    // =========================================================================
    {
        .osc = {
            .freq = 262,
            .harmonics = 1,
            .phase = {0},
            .phase_incr = 0,
            .ampl = {400},
            .wave = sqr,
            .state = true
        },

        .adsr = {
            .attack_percent = 0,
            .decay_percent = 25,
            .sustain_level = 30000,
            .release_percent = 5
        },

        .seq = {
            .seq_on = 1,
            .note_change = 0,
            .seq_idx = 0,
            .steps = 64,
            .step_ms = 200,
            .ms_count = 0,
            .sequence = {
                0,0,
                208,
                220,
                247,0,

                220,
                208,
                165,0,

                165,
                220,
                262,0,

                247,
                220,
                208,0,

                0,
                220,
                247,0,
                262,0,

                220,0,
                165,0,

                165,0,0,0,

                0,
                220,0,

                294,
                349,0,

                330,
                294,

                0,0,

                0,
                220,
                262,0,

                247,
                220,

                208,0,

                208,
                220,
                247,0,

                262,0,

                220,0,
                165,0,

                165,0,0,0
            }
        }
    },

    // =========================================================================
    // Voice 2 - unused by default (synth_active_voices=2), configurable later
    // =========================================================================
    {
        .osc = {
            .freq = 440,
            .harmonics = 1,
            .phase = {0},
            .phase_incr = 0,
            .ampl = {0},
            .wave = sine,
            .state = true
        },

        .adsr = {
            .attack_percent = 5,
            .decay_percent = 15,
            .sustain_level = 6554,
            .release_percent = 20
        },

        .seq = {
            .seq_on = 0,
            .note_change = 0,
            .seq_idx = 0,
            .steps = 8,
            .step_ms = 250,
            .ms_count = 0,
            .sequence = {
                440, 440, 440, 440, 440, 440, 440, 440
            }
        }
    },

    // =========================================================================
    // Voice 3 - unused by default (synth_active_voices=2), configurable later
    // =========================================================================
    {
        .osc = {
            .freq = 440,
            .harmonics = 1,
            .phase = {0},
            .phase_incr = 0,
            .ampl = {0},
            .wave = sine,
            .state = true
        },

        .adsr = {
            .attack_percent = 5,
            .decay_percent = 15,
            .sustain_level = 6554,
            .release_percent = 20
        },

        .seq = {
            .seq_on = 0,
            .note_change = 0,
            .seq_idx = 0,
            .steps = 8,
            .step_ms = 250,
            .ms_count = 0,
            .sequence = {
                440, 440, 440, 440, 440, 440, 440, 440
            }
        }
    }
};

voice_t additive_voice = {
    .osc = {
        .freq = 261,
        .harmonics = MAX_HARMONICS,
        .phase = {0},
        .phase_incr = 0,
        .ampl = {
            300, 150, 100, 75, 60, 50, 42,
            10, 5, 10, 10, 5, 10, 20
        },
        .wave = sine,
        .state = true
    },

    .adsr = {
            .attack_percent = 20,
            .decay_percent = 50,
            .sustain_level = 50000,
            .release_percent = 10
        },

    .seq = {
        .seq_on = 0,
        .note_change = 0,
        .seq_idx = 0,
        .steps = 8,
        .step_ms = 150,
        .ms_count = 0,
        .sequence = {
            100, 200, 300, 400, 500, 50, 100, 1000
        }
    }
};

uint16_t synth_next_sample(void){
/*
 * Generate one output sample.
 * Audio signal path:
 *
 * Oscillator
 *     ↓
 * ADSR envelope
 *     ↓
 * Mix active voices
 *     ↓
 * LFO amplitude modulation (optional)
 *     ↓
 * DAC offset (bipolar -> unipolar)
 */

    int32_t ret = 0;
    int32_t tmp = 0;
    uint16_t adsr_level = 0;

    if(synth_mode == ADDITIVE){
        ret = osc_next_sample(&additive_voice.osc);
        adsr_level = adsr_next_sample(&additive_voice.adsr);
        ret = (ret * adsr_level) >> Q16_SHIFT;
    }
    else if(synth_mode == WAVETABLE){
        for(uint32_t i=0; i<synth_active_voices; i++){
            tmp = osc_next_sample(&wavetable_voices[i].osc);
            adsr_level = adsr_next_sample(&wavetable_voices[i].adsr);
            ret += (tmp * adsr_level) >> Q16_SHIFT;
        }
    }

    if(lfo.mode==AMPL){
        uint16_t lfo_level = lfo_next_sample();
        ret = (ret * lfo_level) >> Q16_SHIFT;
    }
    return (int16_t)ret + DAC_OFFSET;
}

void synth_process(void){
/*
 * Process pending note changes in the main loop.
 */
    if(synth_mode == ADDITIVE){
        seq_process(&additive_voice.seq, &additive_voice.osc, &additive_voice.adsr);
    }
    else if(synth_mode == WAVETABLE){
        for(uint32_t i=0; i<synth_active_voices; i++){
            seq_process(&wavetable_voices[i].seq, &wavetable_voices[i].osc, &wavetable_voices[i].adsr);        }
    }
}

void synth_start(void){
    if(synth_mode == ADDITIVE) {
        adsr_update(&additive_voice.adsr, additive_voice.seq.step_ms);
        adsr_trigger(&additive_voice.adsr);
        seq_start(&additive_voice.seq);
    }
    else if (synth_mode == WAVETABLE) {
        for(uint32_t i = 0; i < synth_active_voices; i++) {
            adsr_update(&wavetable_voices[i].adsr, wavetable_voices[i].seq.step_ms);
            adsr_trigger(&wavetable_voices[i].adsr);
            seq_start(&wavetable_voices[i].seq);
        }
    }
    tc_start(TC0, 1);
}

void synth_stop(void){
    seq_stop(&additive_voice.seq);
    for(uint32_t i = 0; i < MAX_VOICES; i++) {
            seq_stop(&wavetable_voices[i].seq);
        }
    tc_stop(TC0, 1);
}

void seq_timer_setup(void){
/*
 * Configure Timer Counter 1 to generate a note-change event
 * at the selected step interval.
 */
	step_rc = SystemCoreClock / 2 / MS_IN_S * 1 - 1; // Timer period corresponding to one millisecond.
	sysclk_enable_peripheral_clock(ID_TC1);
	tc_init(TC0, 1, TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC);
	NVIC_EnableIRQ(TC1_IRQn);
	tc_enable_interrupt(TC0, 1, TC_IER_CPCS);
	tc_write_rc(TC0, 1, step_rc);
}

void TC1_Handler(void)
{
    tc_get_status(TC0, 1);
    if (synth_mode == ADDITIVE) {
        seq_t *seq = &additive_voice.seq;
        if (seq->seq_on && seq->steps) {
            seq->ms_count++;
            if (seq->ms_count >= seq->step_ms) {
                seq->ms_count = 0;
                seq->note_change = 1;
            }
        }
    }

    else if (synth_mode == WAVETABLE) {
        for (uint32_t i = 0; i < synth_active_voices; i++) {
            seq_t *seq = &wavetable_voices[i].seq;
            if (!seq->seq_on || !seq->steps)
                continue;
            seq->ms_count++;
            if (seq->ms_count >= seq->step_ms) {
                seq->ms_count = 0;
                seq->note_change = 1;
            }
        }
    }
}

