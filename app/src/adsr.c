#include <adsr.h>

void adsr_trigger(adsr_t *adsr){
	adsr->adsr_idx = 0;
	adsr->adsr_level_fp = 0;
	adsr->state = ATTACK;
}

void adsr_update(adsr_t *adsr, uint32_t step_ms){
/*
 * Recalculate envelope durations and fixed-point increments
 * after ADSR parameters or sequencer step duration change.
 */
	uint32_t step_samples = ((uint64_t)step_ms*FS)/MS_IN_S;

	adsr->attack_samples = ((uint64_t)step_samples*adsr->attack_percent)/100;
	adsr->decay_samples = ((uint64_t)step_samples*adsr->decay_percent)/100;
	adsr->release_samples = ((uint64_t)step_samples*adsr->release_percent)/100;

	adsr->attack_incr_fp = adsr->attack_samples ? (((uint32_t)ADSR_MAX_LEVEL<<Q16_SHIFT) / adsr->attack_samples) : 0;
	adsr->decay_decr_fp = adsr->decay_samples ? ((((uint32_t)ADSR_MAX_LEVEL-adsr->sustain_level)<<Q16_SHIFT) / adsr->decay_samples) : 0;
	adsr->release_decr_fp = adsr->release_samples ? (((uint32_t)adsr->sustain_level<<Q16_SHIFT) / adsr->release_samples) : 0;

	uint32_t used_samples = adsr->attack_samples + adsr->decay_samples + adsr->release_samples;

	if(used_samples>=step_samples){
		adsr->sustain_samples = 0;
	}
	else{
		adsr->sustain_samples = step_samples-used_samples;
	}
}

uint16_t adsr_next_sample(adsr_t *adsr){
/*
 * Advance the ADSR state machine and return the current gain.
 */
	switch(adsr->state){
		case ATTACK:
			adsr->adsr_level_fp+=adsr->attack_incr_fp;
			if(adsr->adsr_level_fp > (ADSR_MAX_LEVEL<<Q16_SHIFT)){
				adsr->adsr_level_fp = ADSR_MAX_LEVEL<<Q16_SHIFT;
			}
			adsr->adsr_idx++;
			if(adsr->adsr_idx>=adsr->attack_samples || adsr->attack_samples==0){

				adsr->adsr_level_fp=ADSR_MAX_LEVEL<<Q16_SHIFT;
				adsr->adsr_idx=0;
				adsr->state=DECAY;
			}
		break;
		case DECAY:
			if(adsr->adsr_level_fp > adsr->decay_decr_fp){
				adsr->adsr_level_fp-=adsr->decay_decr_fp;
			}
			else{
				adsr->adsr_level_fp=(uint32_t)adsr->sustain_level<<Q16_SHIFT;
			}
			adsr->adsr_idx++;
			if(adsr->adsr_idx>=adsr->decay_samples || adsr->decay_samples==0){
				adsr->adsr_idx=0;
				adsr->adsr_level_fp=(uint32_t)adsr->sustain_level<<Q16_SHIFT;
				adsr->state=SUSTAIN;
			}
		break;

		case SUSTAIN:
			if(adsr->sustain_samples==0){
				adsr->adsr_idx=0;
				adsr->state=RELEASE;
				break;
			}
			adsr->adsr_level_fp=((uint32_t)adsr->sustain_level)<<Q16_SHIFT;
			adsr->adsr_idx++;

			if(adsr->adsr_idx>=adsr->sustain_samples){
				adsr->adsr_idx=0;
				adsr->state=RELEASE;
			}
		break;

		case RELEASE:
			if(adsr->adsr_level_fp > adsr->release_decr_fp){
				adsr->adsr_level_fp-=adsr->release_decr_fp;
			}
			else{
				adsr->adsr_level_fp=0;
			}

			adsr->adsr_idx++;

			if(adsr->adsr_idx>=adsr->release_samples || adsr->release_samples==0){

				adsr->adsr_idx=0;
				adsr->adsr_level_fp=0;
				adsr->state=IDLE;
			}
		break;

		case IDLE:
			adsr->adsr_idx=0;
			adsr->adsr_level_fp=0;
		break;
	}
	return (uint16_t)(adsr->adsr_level_fp>>Q16_SHIFT);
}
