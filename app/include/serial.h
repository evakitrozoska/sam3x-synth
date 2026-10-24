#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include <uart.h>
#include <stdint.h>
#include <pio.h>
#include <test_board.h>
#include <fifo.h>

void uart_setup(uint32_t baud, uint32_t par);

#endif // SERIAL_H_INCLUDED
