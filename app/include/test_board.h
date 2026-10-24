#ifndef TEST_BOARD_H_INCLUDED 
#define TEST_BOARD_H_INCLUDED
#include <asf.h>
void init_btn_leds(void);
int get_button_state(void);
int get_button_press(void);

typedef struct{
    Pio*port;       
    uint32_t mask;  
    uint32_t idx;   
}gpio_t;

extern gpio_t T1, D1, T2, D2, T3, D3, T4, D4;

void adc_setup(void);
uint32_t adc_read(void);

#endif // TEST_BOARD_H_INCLUDED
