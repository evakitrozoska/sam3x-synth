#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#define FS 42000

#define LUT_SIZE 512
#define LUT_BITS 9

#define BUF_SIZE 128

#define PHASE_BITS 32

#define MS_IN_S 1000

#define Q16_SHIFT 16 // ADSR and LFO levels are represented as Q16.16 fixed-point value (0...65535)

#define AMP_SHIFT 12 // wavetable samples use 12-bit signed range (-2048...2047)

#define DAC_OFFSET 2048

#endif // CONFIG_H_INCLUDED
