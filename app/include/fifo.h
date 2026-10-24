#ifndef FIFO_H_INCLUDED
#define FIFO_H_INCLUDED

#include <stdint.h>

#define FIFO_SIZE 64

typedef struct{
    uint32_t size;
    uint8_t *data;
    uint32_t write_idx;
    uint32_t read_idx;
}fifo_t;

uint32_t fifo_read(fifo_t *fifo, uint8_t *data, uint32_t n);
uint32_t fifo_write(fifo_t *fifo, const uint8_t *data, uint32_t n);

#endif // FIFO_H_INCLUDED
