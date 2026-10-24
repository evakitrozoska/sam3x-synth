#include <asf.h>
#include <test_board.h>
#include <synth.h>
#include <audio.h>
#include <sequencer.h>
#include <config.h>
#include <serial.h>
#include <fifo.h>
#include <parser.h>

#ifdef __cplusplus
extern "C" {
#endif

extern fifo_t fifo_serial;

int main (void)
{
    /* sets the processor clock according to conf_clock.h definition */
    sysclk_init();
    /* disable watchdog */
    WDT->WDT_MR = WDT_MR_WDDIS;

    /********************* HW init     ***************************/
    init_btn_leds();
    dac_setup();
    pdc_timer_setup();
    pdc_setup();
    seq_timer_setup();
    synth_start();
    uart_setup(115200, UART_MR_PAR_NO);

/********************* Main loop     ***************************/
    while(1){
        synth_process();
        fill_buffer();
        uint8_t byte;
        if(fifo_read(&fifo_serial, &byte, 1)){
            protocol_parse_byte(byte);
        }
	}
}

#ifdef __cplusplus

}

#endif
