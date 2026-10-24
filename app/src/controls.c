#include <controls.h>
#include <synth.h>

extern synth_mode_t synth_mode;
extern voice_t wavetable_voices[];
extern voice_t additive_voice;
extern uint32_t synth_active_voices;
extern lfo_t lfo;

void update_controls(packet_t *packet){
	switch(packet->module){
		case MODULE_ENGINE:
			engine_control(packet);
			break;

		case MODULE_OSC:
			osc_control(packet);
			break;

		case MODULE_ADSR:
			adsr_control(packet);
			break;

		case MODULE_SEQ:
			seq_control(packet);
			break;

		case MODULE_LFO:
			lfo_control(packet);
			break;

		case MODULE_HARM:
			harm_control(packet);
			break;

		default:
			break;
	}
}

void engine_control(packet_t *packet){
	switch(packet->parameter){
		case ENGINE_MODE:
			synth_stop();
			synth_mode = (synth_mode_t)packet->value;
			synth_start();
			break;

		case ENGINE_ACTIVE_VOICES:
			if(packet->value <= MAX_VOICES){
				synth_stop();
				synth_active_voices = packet->value;
				synth_start();
			}
			break;

		default:
			break; // no action
	}
}

void osc_control(packet_t *packet){
	voice_t *voice;

	if(synth_mode == ADDITIVE){
		return;
	}
	else if(synth_mode == WAVETABLE){
		if(packet->voice_id >= MAX_VOICES)
			return;

		voice = &wavetable_voices[packet->voice_id];
	}
	else{
		return;
	}

	switch(packet->parameter){
		case OSC_VOLUME:
			voice->osc.ampl[0] = packet->value;
			break;

		case OSC_WAVEFORM:
			switch(packet->value){
				case 0:
					voice->osc.wave = sine;
					break;

				case 1:
					voice->osc.wave = saw;
					break;

				case 2:
					voice->osc.wave = sqr;
					break;
			}
			break;

		default:
			break; // no action
	}
}

void adsr_control(packet_t *packet){
	voice_t *voice;

	if(synth_mode == ADDITIVE){
		voice = &additive_voice;
	}
	else if(synth_mode == WAVETABLE){
		if(packet->voice_id >= MAX_VOICES)
			return;

		voice = &wavetable_voices[packet->voice_id];
	}
	else{
		return;
	}

	switch(packet->parameter){
		case ADSR_ATTACK:
			voice->adsr.attack_percent = packet->value;
			adsr_update(&voice->adsr, voice->seq.step_ms);
			break;

		case ADSR_DECAY:
			voice->adsr.decay_percent = packet->value;
			adsr_update(&voice->adsr, voice->seq.step_ms);
			break;

		case ADSR_SUSTAIN:
			voice->adsr.sustain_level = packet->value;
			adsr_update(&voice->adsr, voice->seq.step_ms);
			break;

		case ADSR_RELEASE:
			voice->adsr.release_percent = packet->value;
			adsr_update(&voice->adsr, voice->seq.step_ms);
			break;

		default:
			break; // no action
	}
}

void seq_control(packet_t *packet){
	voice_t *voice;

	if(synth_mode == ADDITIVE){
		voice = &additive_voice;
	}
	else if(synth_mode == WAVETABLE){
		if(packet->voice_id >= MAX_VOICES)
			return;

		voice = &wavetable_voices[packet->voice_id];
	}
	else{
		return;
	}

	switch(packet->parameter){
		case SEQ_STATE:
			voice->seq.seq_on = packet->value;
			break;

		case SEQ_STEPS:
			if(packet->value <= MAX_STEPS){
				voice->seq.steps = packet->value;
			}
			break;

		case SEQ_STEP_TIME:
			voice->seq.step_ms = packet->value;
			adsr_update(&voice->adsr, voice->seq.step_ms);
			break;

		case SEQ_NOTE:
			if(packet->index < MAX_STEPS){
				voice->seq.sequence[packet->index] = packet->value;
			}
			break;

		default:
			break; // no action
	}
}

void lfo_control(packet_t *packet){
	switch(packet->parameter){
		case LFO_DESTINATION:
			if(packet->value == AMPL){
				lfo.mode = AMPL;
			}
			else lfo.mode = OFF;
			break;

		case LFO_FREQ:
			set_lfo_freq(packet->value);
			break;

		case LFO_DEPTH:
			lfo.depth = packet->value;
			break;

		case LFO_WAVEFORM:
			switch(packet->value){
				case 0:
					lfo.wave = sine;
					break;

				case 1:
					lfo.wave = saw;
					break;

				case 2:
					lfo.wave = sqr;
					break;
			}
			break;

		default:
			break; // no action
	}
}

void harm_control(packet_t *packet){
	if(synth_mode != ADDITIVE)
		return;

	switch(packet->parameter){

		case HARM_COUNT:
			if(packet->value <= MAX_HARMONICS){
				additive_voice.osc.harmonics = packet->value;
			}
			break;

		case HARM_AMPLITUDE:
			if(packet->index < MAX_HARMONICS){
				additive_voice.osc.ampl[packet->index] = packet->value;
			}
			break;

		default:
			break;
	}
}
