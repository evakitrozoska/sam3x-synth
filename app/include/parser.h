#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <stdint.h>

#define PACKET_BYTES 6

typedef enum{
	WAIT_HEADER,
	READ_PACKET
}parser_state_t;

typedef struct{
	uint8_t module;
	uint8_t voice_id;
	uint8_t parameter;
	uint8_t index;
	uint16_t value;
}packet_t;

void protocol_parse_byte(uint8_t byte);

#endif // PARSER_H_INCLUDED
