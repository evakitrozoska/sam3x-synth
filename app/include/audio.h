#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include <stdint.h>

#include <config.h>

#include <synth.h>
#include <test_board.h>

#include <dacc.h>
#include <pdc.h>

#define TIMER_RC 999   // RC = MCK / 2 / FS - 1
#define TIMER_RA 499   // 50% duty cycle

void pdc_setup(void);
void fill_buffer(void);
void pdc_timer_setup(void);
void dac_setup(void);

#endif // AUDIO_H_INCLUDED
