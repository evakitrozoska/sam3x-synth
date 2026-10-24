#include <test_board.h>

gpio_t T1 = {PIOC, PIO_PC26, PIO_PC26_IDX};
gpio_t T2 = {PIOC, PIO_PC25, PIO_PC25_IDX};
gpio_t T3 = {PIOC, PIO_PC24, PIO_PC24_IDX};
gpio_t T4 = {PIOC, PIO_PC23, PIO_PC23_IDX};

gpio_t D1 = {PIOC, PIO_PC22, PIO_PC22_IDX};
gpio_t D2 = {PIOC, PIO_PC21, PIO_PC21_IDX};
gpio_t D3 = {PIOC, PIO_PC29, PIO_PC29_IDX};
gpio_t D4 = {PIOC, PIO_PD7, PIO_PD7_IDX};

int get_button_state(void){
    int out=0;
    out |= (!ioport_get_pin_level(T1.idx)) << 0;
    out |= (!ioport_get_pin_level(T2.idx)) << 1;
    out |= (!ioport_get_pin_level(T3.idx)) << 2;
    out |= (!ioport_get_pin_level(T4.idx)) << 3;
    return out;
}

int get_button_press(void){

    static int btn_old = 0;
    int btn = get_button_state();
    int out = (btn_old^btn)&btn;
    btn_old = btn;
    return out;
}


void init_btn_leds(void){
    ioport_init();

    ioport_enable_pin(T1.idx);
    ioport_enable_pin(T2.idx);
    ioport_enable_pin(T3.idx);
    ioport_enable_pin(T4.idx);

    ioport_enable_pin(D1.idx);
    ioport_enable_pin(D2.idx);
    ioport_enable_pin(D3.idx);
    ioport_enable_pin(D4.idx);

    ioport_set_pin_dir(T1.idx, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(D1.idx, IOPORT_DIR_OUTPUT);

    ioport_set_pin_dir(T2.idx, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(D2.idx, IOPORT_DIR_OUTPUT);

    ioport_set_pin_dir(T3.idx, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(D3.idx, IOPORT_DIR_OUTPUT);

    ioport_set_pin_dir(T4.idx, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(D4.idx, IOPORT_DIR_OUTPUT);

    ioport_set_pin_mode(T1.idx, IOPORT_MODE_PULLUP | IOPORT_MODE_DEBOUNCE);
    ioport_set_pin_mode(T2.idx, IOPORT_MODE_PULLUP | IOPORT_MODE_DEBOUNCE);

    ioport_set_pin_mode(T3.idx, IOPORT_MODE_PULLUP | IOPORT_MODE_DEBOUNCE);
    ioport_set_pin_mode(T4.idx, IOPORT_MODE_PULLUP | IOPORT_MODE_DEBOUNCE);

    pio_set_debounce_filter(PIOC, T1.mask |T2.mask | T3.mask | T4.mask, 1000);
}


void adc_setup(void){
    sysclk_enable_peripheral_clock(ID_ADC);
    adc_init(ADC, SystemCoreClock, 6400000, ADC_MR_STARTUP_SUT64);
    adc_configure_timing(ADC, 0, ADC_MR_SETTLING_AST17, 1);
    adc_set_resolution(ADC, ADC_MR_LOWRES_BITS_12);
    adc_configure_trigger(ADC, ADC_TRIG_SW, 0);
    adc_enable_channel(ADC, ADC_CHANNEL_7);
}


uint32_t adc_read(void){
    adc_start(ADC);
    int status;

    do{
    status = adc_get_status(ADC);
    }while( (status & ADC_ISR_EOC7) != ADC_ISR_EOC7);

    return adc_get_channel_value(ADC, ADC_CHANNEL_7);

}




