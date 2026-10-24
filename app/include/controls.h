#ifndef CONTROLS_H_INCLUDED
#define CONTROLS_H_INCLUDED

#include <parser.h>

// =====================================================
// MODULES
// =====================================================
#define MODULE_ENGINE   0x01
#define MODULE_OSC      0x02
#define MODULE_ADSR     0x03
#define MODULE_SEQ      0x04
#define MODULE_LFO      0x05
#define MODULE_HARM     0x06

// =====================================================
// ENGINE PARAMETERS
// VOICE_ID = 0xFF
// =====================================================
#define ENGINE_MODE              0x01
#define ENGINE_ACTIVE_VOICES     0x02
#define ENGINE_STATE             0x03

// =====================================================
// OSCILLATOR PARAMETERS
// wavetable synthesis only
// VOICE_ID = selected voice
// =====================================================
#define OSC_VOLUME               0x01
#define OSC_WAVEFORM             0x02

// =====================================================
// HARMONIC PARAMETERS
// Additive synthesis only
// VOICE_ID = 0
// =====================================================
#define HARM_COUNT               0x01
#define HARM_AMPLITUDE           0x02

// =====================================================
// ADSR PARAMETERS
// VOICE_ID = selected voice
// =====================================================
#define ADSR_ATTACK              0x01
#define ADSR_DECAY               0x02
#define ADSR_SUSTAIN             0x03
#define ADSR_RELEASE             0x04

// =====================================================
// SEQUENCER PARAMETERS
// VOICE_ID = selected voice
// =====================================================
#define SEQ_STATE                0x01
#define SEQ_STEPS                0x02
#define SEQ_STEP_TIME            0x03
#define SEQ_NOTE                 0x04

// =====================================================
// LFO PARAMETERS
// VOICE_ID = 0xFF (global)
// =====================================================
#define LFO_DESTINATION          0x01
#define LFO_FREQ                 0x02
#define LFO_DEPTH                0x03
#define LFO_WAVEFORM             0x04

void update_controls(packet_t *packet);
void engine_control(packet_t *packet);
void osc_control(packet_t *packet);
void adsr_control(packet_t *packet);
void seq_control(packet_t *packet);
void lfo_control(packet_t *packet);
void harm_control(packet_t *packet);

#endif // CONTROLS_H_INCLUDED
