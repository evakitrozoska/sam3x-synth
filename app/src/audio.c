#include <audio.h>

/*
 * Ping-pong buffers:
 * One buffer is consumed by the DAC through PDC DMA while the CPU fills
 * the other buffer. When the DAC finishes a buffer, the DAC ISR swaps
 * the roles of the two buffers.
 */

volatile uint16_t buf1[BUF_SIZE] = {0};
volatile uint16_t buf2[BUF_SIZE] = {0};
volatile uint16_t* buf_cpu = buf1; // buffer currently filled by CPU
volatile uint16_t* buf_dma = buf2; // buffer currently sent to DAC
volatile uint16_t cpu_buf_fill_done = 0;

void fill_buffer(void){
/*
 * Generate the next audio block.
 * The CPU must finish filling the buffer before the DAC requests it.
 */
	if(cpu_buf_fill_done==0){
	for(uint16_t i = 0; i < BUF_SIZE; i++){
		buf_cpu[i] = synth_next_sample();
		}
		cpu_buf_fill_done = 1;
	}
}

void DACC_Handler(void){
/*
 * DAC interrupt triggered when the current PDC transfer finishes.
 *
 * If a new CPU-generated buffer is available:
 *   - load it as the next DMA buffer
 *   - swap CPU/DMA buffer pointers
 *
 * Otherwise the CPU failed to generate samples fast enough
 * and an underrun occurred.
 */
    uint32_t status = dacc_get_interrupt_status(DACC);
	if (status & DACC_ISR_ENDTX){
		if(cpu_buf_fill_done==1){
			PDC_DACC->PERIPH_TNPR = (uint32_t)buf_cpu;
			PDC_DACC->PERIPH_TNCR = BUF_SIZE;
            volatile uint16_t* tmp = buf_cpu;
			buf_cpu = buf_dma;
			buf_dma = tmp;
			cpu_buf_fill_done = 0;
		}
		else{
            // CPU did not prepare the next block in time
			ioport_toggle_pin_level(D3.idx);
			PDC_DACC->PERIPH_TNPR = (uint32_t)buf_dma;
			PDC_DACC->PERIPH_TNCR = BUF_SIZE;
		}
	}
}

void pdc_setup(void){
/*
 * Initialize the PDC for double-buffered DMA transfers.
 * Once TCR reaches zero, the PDC automatically switches to
 * the buffer specified by TNPR/TNCR.
 */
    for(uint16_t i = 0; i < BUF_SIZE; i++){
		buf_cpu[i] = 0;
		buf_dma[i] = 0;
    }
    PDC_DACC->PERIPH_TPR = (uint32_t)buf_cpu;        // Transmit Pointer Register
    PDC_DACC->PERIPH_TCR = BUF_SIZE;                 // Transmit Counter Register
    PDC_DACC->PERIPH_TNPR = 0;                       // Transmit Next Pointer Register
    PDC_DACC->PERIPH_TNCR = 0;                       // Transmit Next Counter Pointer Register
    pdc_enable_transfer(PDC_DACC, PERIPH_PTCR_TXTEN);
}

void pdc_timer_setup(void){
/*
 * TCLK = MCK/2
 * Configure Timer Counter 0 to generate the DAC trigger.
 * TIOA outputs a square wave at the sampling frequency.
 */
   sysclk_enable_peripheral_clock(ID_TC0);
   tc_init(TC0, 0, TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_ACPC_CLEAR | TC_CMR_ACPA_SET);
   pio_set_peripheral(PIOB, PIO_PERIPH_B, PIO_PB25B_TIOA0);
   tc_write_rc(TC0, 0, TIMER_RC);
   tc_write_ra(TC0, 0, TIMER_RA);
   tc_start(TC0, 0);
}

void dac_setup(void){
/*
 * Enable hardware triggering from TIOA and generate an interrupt
 * whenever the current DMA buffer has been transmitted.
 */
    sysclk_enable_peripheral_clock(ID_DACC);
    dacc_reset(DACC);
    dacc_set_transfer_mode(DACC, DACC_MR_WORD_HALF);
    dacc_set_timing(DACC, 0x08, 0, 0x10); // refresh, max speed, startup time
    dacc_set_channel_selection(DACC, 1);
    dacc_enable_channel(DACC, 1);

    dacc_set_trigger(DACC, 1); // TRGSEL = 1 => TIO Output of the Timer Counter Channel 0
    NVIC_ClearPendingIRQ(DACC_IRQn);
    NVIC_EnableIRQ(DACC_IRQn);
    dacc_enable_interrupt(DACC, DACC_IER_ENDTX);
    // ENDTX is raised when the current DMA transfer completes (TCR reaches zero).
}

