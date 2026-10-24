#ifndef ADSR_H_INCLUDED
#define ADSR_H_INCLUDED

#include <stdint.h>
#include <config.h>

#define ADSR_MAX_LEVEL 65535UL

typedef enum {
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE,
	IDLE
}adsr_state_t;

typedef struct {
    uint16_t attack_percent;
    uint16_t decay_percent;
    uint16_t sustain_level;
    uint16_t release_percent;

    adsr_state_t state;

    uint32_t adsr_idx;

    uint32_t attack_samples;
    uint32_t decay_samples;
    uint32_t release_samples;
    uint32_t sustain_samples;

    uint32_t adsr_level_fp;

    uint32_t attack_incr_fp;
    uint32_t decay_decr_fp;
    uint32_t release_decr_fp;

} adsr_t;

void adsr_trigger(adsr_t *adsr);
void adsr_update(adsr_t *adsr, uint32_t step_ms);
uint16_t adsr_next_sample(adsr_t *adsr);

#endif // ADSR_H_INCLUDED
