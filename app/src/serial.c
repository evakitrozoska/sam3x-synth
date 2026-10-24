#include <serial.h>

extern fifo_t fifo_serial;

void uart_setup(uint32_t baud, uint32_t par){
    sysclk_enable_peripheral_clock(ID_PIOA);
    pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA8A_URXD | PIO_PA9A_UTXD); // Za pina PA8 (RX) in PA9 (TX) nastavimo funkcijo preiferne enote A (glej datasheet, PIO)
    pio_pull_up(PIOA, PIO_PA8A_URXD | PIO_PA9A_UTXD, PIO_PULLUP);

    sam_uart_opt_t uart_opts = {
        .ul_mck = SystemCoreClock,
        .ul_baudrate = baud,
        .ul_mode = UART_MR_CHMODE_NORMAL | par
    };

    sysclk_enable_peripheral_clock(ID_UART);
    uart_init(UART, &uart_opts);
    uart_reset(UART);
    uart_reset_status(UART);

    NVIC_EnableIRQ(UART_IRQn);
    uart_enable_interrupt(UART, UART_IER_RXRDY);
    uart_enable(UART);
}

void UART_Handler(void){
    uint32_t status = uart_get_status(UART);
    if(status & (UART_SR_OVRE|UART_SR_FRAME|UART_SR_PARE) ){
        ioport_set_pin_level(D2.idx, 1);
            while(1); //hang
    }
    uint8_t byte;
    uart_read(UART, &byte);
    fifo_write(&fifo_serial, &byte, 1);
}
