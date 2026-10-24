#include <fifo.h>

uint8_t fifo_buff[FIFO_SIZE];

fifo_t fifo_serial = {
    .size = FIFO_SIZE,
    .data = fifo_buff,
    .read_idx = 0,
    .write_idx = 0,
};

uint32_t fifo_read(fifo_t *fifo, uint8_t *data, uint32_t n){
	uint32_t i = 0;
	for(i = 0; i < n; i++){
		if(fifo->read_idx != fifo->write_idx){ // if not empty
			data[i] = fifo->data[fifo->read_idx];
			fifo->read_idx = (fifo->read_idx + 1) % fifo->size;
		}
		else{
			break;
		}
	}
	return i;
}

uint32_t fifo_write(fifo_t *fifo, const uint8_t *data, uint32_t n){
	uint32_t i = 0;
	for(i = 0; i < n; i++){
		uint32_t w_new = (fifo->write_idx + 1) % fifo->size;
		if(w_new != fifo->read_idx){ // if not full
			fifo->data[fifo->write_idx] = data[i];
			fifo->write_idx = w_new;
		}
		else{
			break;
		}
	}
	return i;
}


