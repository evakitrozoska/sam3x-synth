// Olson - Boards of Canada - two voices

voice_t wavetable_voices[MAX_VOICES] = {
    {
        .osc = {
            .freq = 880,
            .harmonics = 1,
            .phase = {0},
            .phase_incr = 0,
            .ampl = {
                500
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
            .steps = 32,
            .step_ms = 400,
            .ms_count = 0,
            .sequence = {
                0,
                660, 588, 660, 524, 660, 588, 660,
                588, 524, 494,
                0, 0, 0, 0, 0, 0,
                740, 660, 740, 880,
                0,
                740,
                0,
                660,
                0, 0, 0, 0, 0, 0, 0
            }
        }
    },

    {
        .osc = {
            .freq = 220,
            .harmonics = 1,
            .phase = {0},
            .phase_incr = 0,
            .ampl = {
                500
            },
            .wave = sine,
            .state = true
        },

        .adsr = {
            .attack_percent = 20,
            .decay_percent = 6,
            .sustain_level = 50000,
            .release_percent = 20
        },

        .seq = {
            .seq_on = 0,
            .note_change = 0,
            .seq_idx = 0,
            .steps = 4,
            .step_ms = 3200,
            .ms_count = 0,
            .sequence = {
                260,   // slightly flat C4
                194,   // slightly flat G3
                294,   // D4
                220    // A3
            }
        }
    }
};